//
// Created by xianglizhao on 2018/5/17.
//

#include "bloomfilter.h"
#include "hash.h"
#include <cmath>

#define PAGESIZE 4096

uint64_t BloomFilter::CalcM(uint64_t nitems, double errorRate) {
    return uint64_t(ceil(-1 * double(nitems) * log(errorRate) / pow(log(2), 2)));
}

int BloomFilter::CalcK(uint64_t mcap, uint64_t nitems) {
    return int(ceil(log(2) * double(mcap) / double(nitems)));
}

uint64_t BloomFilter::CalcBytes(uint64_t mcap) {
    return uint64_t(ceil((mcap + PAGESIZE) / 8));
}

BloomFilter::BloomFilter(
        const string &path, uint64_t itemMaxCounts, double errorRate, HashFunc func, unsigned int seed)
    : m_cap(CalcM(itemMaxCounts, errorRate)),
      m_hashes(CalcK(m_cap, itemMaxCounts)),
      m_mapping(path, CalcBytes(m_cap)),
      m_bitsets((BloomFilter::bvalue_pointer)(m_mapping.Data()), m_cap),
      m_seed(seed),
      m_hashFunc(func) {

}

BloomFilter::values_type BloomFilter::Filter(BloomFilter::values_reference contents) {
    values_type results;
    ReadLock lock(m_rwlock);
    for (int i = 0; i < contents.size(); ++i) {
        const string & value = contents[i];
        auto hash = m_hashFunc(m_hashes, m_seed, m_cap);
        hash->Sum(value);
        for (int j = 0; j < m_hashes; ++j) {
            if (!m_bitsets.test((*hash)[j])) {
                results.push_back(value);
                break;
            }
        }
    }

    return results;
}

void BloomFilter::Update(BloomFilter::values_reference contents) {
    WriteLock lock(m_rwlock);
    for (int i = 0; i < contents.size(); ++i) {
        const string & value = contents[i];
        auto hash = m_hashFunc(m_hashes, m_seed, m_cap);
        hash->Sum(value);
        for (int j = 0; j < m_hashes; ++j) {
            m_bitsets.set((*hash)[j]);
        }
    }
}

void BloomFilter::Delete(BloomFilter::values_reference contents) {
    WriteLock lock(m_rwlock);
    for (int i = 0; i < contents.size(); ++i) {
        const string & value = contents[i];
        auto hash = m_hashFunc(m_hashes, m_seed, m_cap);
        hash->Sum(value);
        for (int j = 0; j < m_hashes; ++j) {
            m_bitsets.reset((*hash)[j]);
        }
    }
}

bool BloomFilter::FilterOne(BloomFilter::value_reference content) {
    ReadLock lock(m_rwlock);
    auto hash = m_hashFunc(m_hashes, m_seed, m_cap);
    hash->Sum(content);
    for (int j = 0; j < m_hashes; ++j) {
        if (!m_bitsets.test((*hash)[j])) {
            return false;
        }
    }
    return true;
}

void BloomFilter::UpdateOne(BloomFilter::value_reference content) {
    WriteLock lock(m_rwlock);
    auto hash = m_hashFunc(m_hashes, m_seed, m_cap);
    hash->Sum(content);
    for (int j = 0; j < m_hashes; ++j) {
        m_bitsets.set((*hash)[j]);
    }
}

void BloomFilter::DeleteOne(BloomFilter::value_reference content) {
    WriteLock lock(m_rwlock);
    auto hash = m_hashFunc(m_hashes, m_seed, m_cap);
    hash->Sum(content);
    for (int j = 0; j < m_hashes; ++j) {
        m_bitsets.reset((*hash)[j]);
    }
}

void BloomFilter::Reset() {
    WriteLock lock(m_rwlock);
    m_mapping.Clear();
}
