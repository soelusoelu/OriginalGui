#pragma once

#include <vector>

struct GuiBitVector {
    std::vector<unsigned> storage;

    void create(int sz);
    void clear();
    bool testBit(int n) const;
    void setBit(int n);
    void clearBit(int n);

private:
    bool arrayTestBit(const unsigned* ary, int n) const;
    void arrayClearBit(unsigned* ary, int n);
    void arraySetBit(unsigned* ary, int n);
    void arraySetBitRange(unsigned* ary, int n1, int n2);
};
