//
// Created by xianglizhao on 2018/6/27.
//

#include "pfconfig.h"

PartitionFilterConfig::PartitionFilterConfig() {

}

bool PartitionFilterConfig::Read(const string &configFile) {
    ptree pt;
    read_json(configFile, pt);
    m_instancePath = pt.get<string>("instance_path");
    m_bloomPath = pt.get<string>("bloom_path");
    m_bloomMaxItemSize = pt.get<uint64_t>("bloom_max_items");
    m_bloomErrorRate = pt.get<double>("bloom_error_rate");
    m_bloomHashSeed = pt.get<uint>("bloom_hash_seed");
    m_bloomMaxPartitions = pt.get<size_t>("bloom_max_partitions");
    m_bloomHashFunc = pt.get<string>("bloom_hash_func");
    return true;
}

size_t PartitionFilterConfig::BloomMaxPartitions() const {
    return m_bloomMaxPartitions;
}

const string &PartitionFilterConfig::InstancePath() const {
    return m_instancePath;
}

const string &PartitionFilterConfig::BloomPath() const {
    return m_bloomPath;
}

uint64_t PartitionFilterConfig::BloomMaxItems() const {
    return m_bloomMaxItemSize;
}

double PartitionFilterConfig::BloomErrorRate() const {
    return m_bloomErrorRate;
}

unsigned int PartitionFilterConfig::BloomHashSeed() const {
    return m_bloomHashSeed;
}

const string& PartitionFilterConfig::BloomHashFunc() const {
    return m_bloomHashFunc;
}

void PartitionFilterConfig::Print() const {
    std::cout << "paxos instance path: " << m_instancePath << std::endl;
    std::cout << "bloom data path: " << m_bloomPath << std::endl;
    std::cout << "bloom max partitions: " << m_bloomMaxPartitions << std::endl;
    std::cout << "bloom max item counts: " << m_bloomMaxItemSize << std::endl;
    std::cout << "bloom error rate: " << m_bloomErrorRate << std::endl;
    std::cout << "bloom hash seed: " << m_bloomHashSeed << std::endl;
}