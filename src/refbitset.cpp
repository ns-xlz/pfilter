//
// Created by xianglizhao on 2018/5/16.
//

#include "refbitset.h"
#include <iostream>
using namespace std;

#define NUMSIZE (sizeof(unsigned long long) << 3)

RefBitset::RefBitset(value_pointer refAlooc, uint64_t allocLength)
    : m_bits(refAlooc), m_lens(allocLength) {

}

void RefBitset::set(uint64_t pos) {
    if (pos >= m_lens) {
        std::cout<<"pos bigger than len";
        return;
    }
    uint64_t x = pos/NUMSIZE, y = pos%NUMSIZE;

    m_bits[x] |= (1ULL<<y);
}

void RefBitset::reset(uint64_t pos) {
    if (pos >= m_lens) {
        return;
    }
    uint64_t x = pos/NUMSIZE, y = pos%NUMSIZE;

    m_bits[x] &= ~(1ULL<<y);
}

bool RefBitset::test(uint64_t pos) {
    if (pos >= m_lens) {
        std::cout<<"pos bigger than len";
        return false;
    }
    uint64_t x = pos/NUMSIZE, y = pos%NUMSIZE;
    return !!(m_bits[x] & (1ULL<<y));
}
