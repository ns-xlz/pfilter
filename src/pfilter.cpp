//
// Created by xianglizhao on 2018/5/18.
//

#include "pfilter.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/filesystem.hpp>
#include <functional>
#include <sys/types.h>
#include <dirent.h>

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::algorithm;
using namespace boost::filesystem;

static ptime TIMESTAMP_START_PTIME(boost::gregorian::date(1970, boost::gregorian::Jan, 1));

Partition::Partition(const string &partition, uint64_t weight)
        : m_partition(partition), m_weight(weight) {

}

const string &Partition::Partition_() const {
    return m_partition;
}

uint64_t Partition::Weight() const {
    return m_weight;
}

bool Partition::operator<(const Partition &p) const {
    return m_weight < p.m_weight;
}

bool Partition::operator<=(const Partition &p) const {
    return m_weight <= p.m_weight;
}

bool Partition::operator>(const Partition &p) const {
    return m_weight > p.m_weight;
}

bool Partition::operator>=(const Partition &p) const {
    return m_weight >= p.m_weight;
}

bool Partition::operator==(const Partition &p) const {
    return (m_partition == p.m_partition) && (m_weight == p.m_weight);
}

uint64_t Partition::operator*() const {
    return m_weight;
}

PartitionManager::PartitionManager(size_t size)
        : m_size(size) {

}

void PartitionManager::AddPartition(const Partition &partition) {
    m_manager.push_back(partition);
    push_heap(m_manager.begin(), m_manager.end(), std::greater<Partition>());
}

Partition PartitionManager::DelMinPartition() {
    pop_heap(m_manager.begin(), m_manager.end(), std::greater<Partition>());
    Partition partition = m_manager.back();
    m_manager.pop_back();
    return partition;
}

bool PartitionManager::IsMinPartition(const Partition &partition) const {
    return m_manager.front() > partition;
}

bool PartitionManager::Full() const {
    return m_manager.size() > m_size;
}

void PartitionManager::Print() const {
    for (vector<Partition>::const_iterator i = m_manager.begin(); i != m_manager.end(); ++i) {
        std::cout << (*i).Weight() << " ";
    }
    std::cout << std::endl;
}

void PartitionManager::DelPartition(const Partition &partition) {
    vector<Partition>::iterator iter = find(m_manager.begin(), m_manager.end(), partition);
    m_manager.erase(iter);
    make_heap(m_manager.begin(), m_manager.end(), std::greater<Partition>());
}

string PartitionFilter::EncodePath(const Partition &partition) const {
//    return Partition(basedir + "/" + partition + ":" + boost::lexical_cast<string>(weight), weight);
    return (boost::filesystem::path(m_bloomPath) / boost::filesystem::path(partition.Partition_())).string()
           + ":" + boost::lexical_cast<string>(partition.Weight());
}

Partition PartitionFilter::DecodePath(const string &path) const {
    vector<string> splits;
    split(splits, path, is_any_of("/:"), token_compress_on);
    uint64_t weight = 0;
    string partition;
    try {
        weight = boost::lexical_cast<uint64_t>(splits[splits.size() - 1]);
        partition = splits[splits.size() - 2];
    } catch (
            boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_lexical_cast> > e) {
        std::cout << "unexpect bloomfile: " << path << std::endl;
    }
    return Partition(partition, weight);
}

void PartitionFilter::AddFilter(const string &partition) {
    WriteLock lock(m_rwlock);
    if (m_filters.find(partition) != m_filters.end()) {
        return;
    }
    uint64_t timestamp = (ptime(microsec_clock::universal_time()) - TIMESTAMP_START_PTIME).total_microseconds();
    AddFilter(Partition(partition, timestamp));
}

void PartitionFilter::AddFilter(const Partition &partition) {
    if (Full()) {
        DelFilter();
    }


    m_filters[partition.Partition_()] = std::shared_ptr<BloomFilter>(
            new BloomFilter(EncodePath(partition), m_maxItems, m_errorRate, m_hashFunc, m_seed));
    m_pm.AddPartition(partition);
}

void PartitionFilter::DelFilter() {
    WriteLock lock(m_rwlock);

    Partition p = m_pm.DelMinPartition();
    if (m_filters.find(p.Partition_()) == m_filters.end()) {
        m_pm.AddPartition(p);
        return;
    }
    m_filters.erase(p.Partition_());
}

void PartitionFilter::DelFilter(const Partition &partition) {
    m_pm.DelPartition(partition);
    m_filters.erase(partition.Partition_());
}

bool PartitionFilter::Full() const {
    return m_pm.Full();
}

PartitionFilter::PartitionFilter(const PartitionFilterConfig &config)
        : m_bloomPath(system_complete(config.BloomPath()).string()),
          m_maxPartitionNum(config.BloomMaxPartitions()),
          m_maxItems(config.BloomMaxItems()),
          m_seed(config.BloomHashSeed()),
          m_errorRate(config.BloomErrorRate()),
          m_pm(config.BloomMaxPartitions()) {
    auto func = g_hashFunc.find(config.BloomHashFunc());
    if (func == g_hashFunc.end()) {
        std::cout << "no such hash func: " << config.BloomHashFunc() << ", use default hash: murmurhash128"
                  << std::endl;
        m_hashFunc = g_hashFunc["murmurhash128"];
    } else {
        m_hashFunc = (*func).second;
    }

    vector<Partition> pm;
//    directory_iterator end;

//    boost::filesystem::path dir = boost::filesystem::path(m_bloomPath);
    DIR * _dir = opendir(m_bloomPath.c_str());

    if (_dir != nullptr) {
        struct dirent *file;
        struct stat sb;

        while((file = readdir(_dir)) != NULL)
        {
            if(strncmp(file->d_name, ".", 1) == 0) {
                continue;
            }
            if(stat(file->d_name, &sb) >= 0 && S_ISREG(sb.st_mode))
            {
                pm.push_back(DecodePath(file->d_name));
            }
        }
        closedir(_dir);
    }
    
    if (pm.size() > 0) {
        sort(pm.begin(), pm.end(), std::greater<Partition>());
        size_t maxsize = m_maxPartitionNum > pm.size() ? pm.size() : m_maxPartitionNum;
        for (size_t i = 0; i < maxsize; ++i) {
            AddFilter(pm[i]);
        }
    }
}

PartitionFilter::values_type PartitionFilter::Filter(PartitionFilter::values_reference contents) {
    PartitionFilter::values_type result = contents;
    ReadLock lock(m_rwlock);
    for (PartitionFilter::const_iterator iter = m_filters.begin();
         iter != m_filters.end(); ++iter) {
        result = iter->second->Filter(result);
    }
    return result;
}

void PartitionFilter::Update(const string &partition, PartitionFilter::values_reference contents) {
    if (m_filters.find(partition) != m_filters.end()) {
        m_filters[partition]->Update(contents);
    } else {
        AddFilter(partition);
        m_filters[partition]->Update(contents);
    }
}

void PartitionFilter::Delete(PartitionFilter::values_reference contents) {
    for (PartitionFilter::const_iterator iter = m_filters.begin();
         iter != m_filters.end(); ++iter) {
        iter->second->Delete(contents);
    }
}

bool PartitionFilter::FilterOne(PartitionFilter::value_reference content) {
    ReadLock lock(m_rwlock);
    for (PartitionFilter::const_iterator iter = m_filters.begin();
         iter != m_filters.end(); ++iter) {
        if (!iter->second->FilterOne(content)) {
            return false;
        }
    }
    return true;
}

void PartitionFilter::UpdateOne(const string &partition, PartitionFilter::value_reference content) {
    if (m_filters.find(partition) != m_filters.end()) {
        m_filters[partition]->UpdateOne(content);
    } else {
        AddFilter(partition);
        m_filters[partition]->UpdateOne(content);
    }
}

void PartitionFilter::DeleteOne(PartitionFilter::value_reference content) {
    for (PartitionFilter::const_iterator iter = m_filters.begin();
         iter != m_filters.end(); ++iter) {
        iter->second->DeleteOne(content);
    }
}

PartitionFilter::~PartitionFilter() {

}

void PartitionFilter::Print() const {
    for (PartitionFilter::const_iterator iter = m_filters.begin();
         iter != m_filters.end(); ++iter) {
        std::cout << iter->first << " ";
    }
    std::cout << std::endl;
}
