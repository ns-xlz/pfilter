//
// Created by xianglizhao on 2018/5/18.
//

#ifndef PFILTER2_PFILTER_H
#define PFILTER2_PFILTER_H

#include <iostream>
#include <vector>
#include <map>
#include "bloomfilter.h"
#include "common.h"
#include "pfconfig.h"

using namespace std;

class Partition {
private:
    string m_partition;
    uint64_t m_weight;
public:
    Partition(const string & partition, uint64_t weight);

    bool operator < (const Partition& p) const;
    bool operator <= (const Partition& p) const;
    bool operator > (const Partition& p) const;
    bool operator >= (const Partition& p) const;
    bool operator == (const Partition& p) const;
    uint64_t operator * () const;

    const string & Partition_() const;
    uint64_t Weight() const;
};

class PartitionManager {
private:
    vector<Partition> m_manager;
    size_t m_size;

public:
    PartitionManager(size_t size);

    void AddPartition(const Partition & partition);
    void DelPartition(const Partition & partition);
    Partition DelMinPartition();
    bool IsMinPartition(const Partition & partition) const;
    bool Full() const;
    void Print() const;
};

class PartitionFilter {
public:
    typedef BloomFilter::values_type        values_type;
    typedef BloomFilter::value_type         value_type;
    typedef BloomFilter::value_reference    value_reference;
    typedef BloomFilter::values_reference   values_reference;

    typedef std::shared_ptr<BloomFilter> storage_value_type;
    typedef map<string, storage_value_type> storage_type;
    typedef storage_type::const_iterator const_iterator;

private:
    boost::shared_mutex m_rwlock;

    string m_bloomPath;
    size_t m_maxPartitionNum;
    uint64_t m_maxItems;
    unsigned int m_seed;
    double m_errorRate;
    HashFunc m_hashFunc;

    storage_type m_filters;
    PartitionManager m_pm;
private:
    void AddFilter(const string & partition);
    void AddFilter(const Partition & partition);
    void DelFilter();
    void DelFilter(const Partition & partition);

    string EncodePath(const Partition & partition) const ;
    Partition DecodePath(const string & path) const;
public:
    PartitionFilter(const PartitionFilterConfig & config);

    values_type Filter(values_reference contents);
    void Update(const string & partition, values_reference contents);
    void Delete(values_reference contents);

    bool FilterOne(value_reference content);
    void UpdateOne(const string & partition, value_reference content);
    void DeleteOne(value_reference content);

    bool Full() const;

    void Print() const;

    ~PartitionFilter();
};


#endif //PFILTER2_PFILTER_H
