//
// Created by xianglizhao on 2018/5/17.
//

#ifndef PFILTER2_HASH_H
#define PFILTER2_HASH_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
using namespace std;

class BloomFilterHash {
public:
    typedef uint64_t result_type;
    typedef string value_type;
    typedef vector<uint64_t> results_type;

protected:
    results_type m_hashNumbers;

public:
    virtual void Sum(const value_type & data);
    virtual result_type operator[] (unsigned int pos) const;
};

typedef boost::shared_ptr<BloomFilterHash> (*HashFunc) (size_t size, unsigned int seed, uint64_t max);
extern std::map<string, HashFunc> g_hashFunc;

class MurmurHash64 : public BloomFilterHash {
protected:
    size_t m_size;
    unsigned int m_seed;
    uint64_t m_max;

public:
    typedef result_type* result_pointer;

    MurmurHash64(size_t size, unsigned int seed, uint64_t max);

    virtual void  Sum(const value_type & data);

    result_type Sum64(const value_type & hashKey) const;

    virtual BloomFilterHash & operator= (const MurmurHash64 & that);
};

class MurmurHash128 : public BloomFilterHash {
protected:
    typedef result_type* result_pointer;
    typedef result_type results_type[4];
    typedef result_type results_half_type[2];

    size_t m_size;
    unsigned int m_seed;
    uint64_t m_max;

    results_type m_h;
public:

    MurmurHash128(size_t size, unsigned int seed, uint64_t max);

    virtual void  Sum(const value_type & data);

    static void Sum128(const value_type & hashKey, unsigned int seed, result_pointer h);

    virtual BloomFilterHash & operator= (const MurmurHash128 & that);

    virtual result_type operator[] (unsigned int pos) const;
};

#endif //PFILTER2_HASH_H
