//
// Created by xianglizhao on 2018/5/16.
//

#ifndef PFILTER2_REFBITSET_H
#define PFILTER2_REFBITSET_H

#include <vector>
#include <ostream>
#include <cstdint>

class RefBitset {
public:
    typedef uint64_t                value_type;
    typedef value_type *            value_pointer;
    typedef value_type &            value_reference;

private:
    value_pointer m_bits;
    uint64_t m_lens;
public:
    RefBitset(value_pointer refAlooc, uint64_t allocLength);

    void set(uint64_t pos);
    void reset(uint64_t pos);

    bool test(uint64_t pos);

};

#endif //PFILTER2_REFBITSET_H
