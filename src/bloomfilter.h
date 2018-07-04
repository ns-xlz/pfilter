//
// Created by xianglizhao on 2018/5/17.
//

#ifndef PFILTER2_BLOOMFILTER_H
#define PFILTER2_BLOOMFILTER_H

#include "refbitset.h"
#include "share_mapping.h"
#include "hash.h"
#include "common.h"

#include <vector>


class BloomFilter {
public:
    typedef string                          value_type;
    typedef const string &                  value_reference;
    typedef vector<value_type>              values_type;
    typedef const values_type &             values_reference;

    typedef RefBitset::value_type       bvalue_type;
    typedef RefBitset::value_pointer    bvalue_pointer;
    typedef RefBitset::value_reference  bvalue_reference;

private:
    boost::shared_mutex m_rwlock;

    uint64_t m_cap;
    int m_hashes;

    SharedMapping m_mapping;
    RefBitset m_bitsets;
    unsigned int m_seed;

    HashFunc m_hashFunc;

    static uint64_t CalcM(uint64_t nitems, double errorRate);
    static int CalcK(uint64_t mcap, uint64_t nitems);
    static uint64_t CalcBytes(uint64_t mcap);

public:
    BloomFilter(const string & path, uint64_t itemMaxCounts, double errorRate, HashFunc func, unsigned int seed=0);

    values_type Filter(values_reference contents);
    void Update(values_reference contents);
    void Delete(values_reference contents);

    bool FilterOne(value_reference content);
    void UpdateOne(value_reference content);
    void DeleteOne(value_reference content);

    void Reset();
};


#endif //PFILTER2_BLOOMFILTER_H
