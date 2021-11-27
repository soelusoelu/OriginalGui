#include "GuiBitVector.h"
#include <cassert>

void GuiBitVector::create(int sz) {
    storage.resize((sz + 31) >> 5);
    memset(storage.data(), 0, storage.size() * sizeof(storage.data()[0]));
}

void GuiBitVector::clear() {
    storage.clear();
}

bool GuiBitVector::testBit(int n) const {
    assert(n < (storage.size() << 5));
    return arrayTestBit(storage.data(), n);
}

void GuiBitVector::setBit(int n) {
    assert(n < (storage.size() << 5));
    return arraySetBit(storage.data(), n);
}

void GuiBitVector::clearBit(int n) {
    assert(n < (storage.size() << 5));
    return arrayClearBit(storage.data(), n);
}

bool GuiBitVector::arrayTestBit(const unsigned* ary, int n) const {
    unsigned mask = static_cast<unsigned>(1) << (n & 31);
    return ((ary[n >> 5] & mask) != 0);
}

void GuiBitVector::arrayClearBit(unsigned* ary, int n) {
    unsigned mask = static_cast<unsigned>(1) << (n & 31);
    ary[n >> 5] &= ~mask;
}

void GuiBitVector::arraySetBit(unsigned* ary, int n) {
    unsigned mask = static_cast<unsigned>(1) << (n & 31);
    ary[n >> 5] |= mask;
}

void GuiBitVector::arraySetBitRange(unsigned* ary, int n1, int n2) {
    while (n1 <= n2) {
        int a_mod = (n1 & 31);
        int b_mod = ((n2 >= n1 + 31) ? 31 : (n2 & 31)) + 1;
        unsigned tmpA = static_cast<unsigned>(((static_cast<unsigned long long>(1) << b_mod) - 1));
        unsigned tmpB = static_cast<unsigned>(((static_cast<unsigned long long>(1) << a_mod) - 1));
        unsigned mask = tmpA & ~tmpB;
        ary[n1 >> 5] |= mask;
        n1 = (n1 + 32) & ~31;
    }
}
