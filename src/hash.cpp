//
// Created by xianglizhao on 2018/5/17.
//

#include "hash.h"

BloomFilterHash::result_type BloomFilterHash::operator[](unsigned int pos) const {
    return m_hashNumbers[pos];
}

void BloomFilterHash::Sum(const value_type &data) {

}

MurmurHash64::MurmurHash64(size_t size, unsigned int seed, uint64_t max)
        : m_size(size), m_seed(seed), m_max(max) {
    m_hashNumbers.resize(size);
}

void MurmurHash64::Sum(const string &data) {
    string hashData(data);
    for (int i = 1; i < m_size; ++i) {
        hashData += (i % 255);
        m_hashNumbers[i] = Sum64(hashData);
    }
}

MurmurHash64::result_type MurmurHash64::Sum64(const string &hashKey) const {
    size_t len = hashKey.size();
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = m_seed ^(len * m);

    const uint64_t *data = (const uint64_t *) hashKey.c_str();
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char *) data;

    switch (len & 7) {
        case 7:
            h ^= uint64_t(data2[6]) << 48;
        case 6:
            h ^= uint64_t(data2[5]) << 40;
        case 5:
            h ^= uint64_t(data2[4]) << 32;
        case 4:
            h ^= uint64_t(data2[3]) << 24;
        case 3:
            h ^= uint64_t(data2[2]) << 16;
        case 2:
            h ^= uint64_t(data2[1]) << 8;
        case 1:
            h ^= uint64_t(data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h % m_max;
}

BloomFilterHash &MurmurHash64::operator=(const MurmurHash64 &that) {
    m_hashNumbers = that.m_hashNumbers;
    m_size = that.m_size;
    m_seed = that.m_seed;
    m_max = that.m_max;
    return *this;
}

// MurmurHash128
inline uint64_t rotl64(uint64_t x, int8_t r) {
    return (x << r) | (x >> (64 - r));
}

#define ROTL64(x, y)    rotl64(x,y)
#define BIG_CONSTANT(x) (x##LLU)

inline uint64_t getblock(const uint64_t *p, int i) {
    return p[i];
}

inline uint32_t fmix(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

inline uint64_t fmix(uint64_t k) {
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
}

MurmurHash128::MurmurHash128(size_t size, unsigned int seed, uint64_t max)
        : m_size(size), m_seed(seed), m_max(max) {

}

void MurmurHash128::Sum(const value_type &data) {
    Sum128(data, m_seed, m_h);
    Sum128(data + char(1), m_seed, m_h + 2);
}

void MurmurHash128::Sum128(const value_type &key, unsigned int seed, result_pointer h) {
    const uint8_t *data = (const uint8_t *) key.c_str();
    const int len = key.size();
    const int nblocks = len / 16;

    uint64_t h1 = seed;
    uint64_t h2 = seed;

    const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
    const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

    const uint64_t *blocks = (const uint64_t *) (data);

    for (int i = 0; i < nblocks; i++) {
        uint64_t k1 = getblock(blocks, i * 2 + 0);
        uint64_t k2 = getblock(blocks, i * 2 + 1);

        k1 *= c1;
        k1 = ROTL64(k1, 31);
        k1 *= c2;
        h1 ^= k1;

        h1 = ROTL64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = ROTL64(k2, 33);
        k2 *= c1;
        h2 ^= k2;

        h2 = ROTL64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    const uint8_t *tail = (const uint8_t *) (data + nblocks * 16);

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15) {
        case 15:
            k2 ^= uint64_t(tail[14]) << 48;
        case 14:
            k2 ^= uint64_t(tail[13]) << 40;
        case 13:
            k2 ^= uint64_t(tail[12]) << 32;
        case 12:
            k2 ^= uint64_t(tail[11]) << 24;
        case 11:
            k2 ^= uint64_t(tail[10]) << 16;
        case 10:
            k2 ^= uint64_t(tail[9]) << 8;
        case 9:
            k2 ^= uint64_t(tail[8]) << 0;
            k2 *= c2;
            k2 = ROTL64(k2, 33);
            k2 *= c1;
            h2 ^= k2;

        case 8:
            k1 ^= uint64_t(tail[7]) << 56;
        case 7:
            k1 ^= uint64_t(tail[6]) << 48;
        case 6:
            k1 ^= uint64_t(tail[5]) << 40;
        case 5:
            k1 ^= uint64_t(tail[4]) << 32;
        case 4:
            k1 ^= uint64_t(tail[3]) << 24;
        case 3:
            k1 ^= uint64_t(tail[2]) << 16;
        case 2:
            k1 ^= uint64_t(tail[1]) << 8;
        case 1:
            k1 ^= uint64_t(tail[0]) << 0;
            k1 *= c1;
            k1 = ROTL64(k1, 31);
            k1 *= c2;
            h1 ^= k1;
    };

    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix(h1);
    h2 = fmix(h2);

    h1 += h2;
    h2 += h1;

    h[0] = h1;
    h[1] = h2;
}

BloomFilterHash &MurmurHash128::operator=(const MurmurHash128 &that) {
    m_hashNumbers = that.m_hashNumbers;
    m_size = that.m_size;
    m_seed = that.m_seed;
    m_max = that.m_max;
    return *this;
}

MurmurHash128::result_type MurmurHash128::operator[](unsigned int pos) const {
    uint64_t p = uint64_t(pos);
    return (m_h[p % 2] + m_h[2 + (((p + (p % 2)) % 4) / 2)] * p) % m_max;
}

boost::shared_ptr<BloomFilterHash> cMurmurHash128(size_t size, unsigned int seed, uint64_t max) {
    return boost::shared_ptr<BloomFilterHash>(new MurmurHash128(size, seed, max));
}

boost::shared_ptr<BloomFilterHash> cMurmurHash64(size_t size, unsigned int seed, uint64_t max) {
    return boost::shared_ptr<BloomFilterHash>(new MurmurHash64(size, seed, max));
}

std::map<string, HashFunc> g_hashFunc = {
        {"murmurhash128", cMurmurHash128},
        {"murmurhash64", cMurmurHash64},
};