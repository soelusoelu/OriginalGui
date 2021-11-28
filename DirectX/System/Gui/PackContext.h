#pragma once

#include <vector>

struct PackContext {
    void* userAllocatorContext = nullptr;
    void* packInfo = nullptr;
    int width = 0;
    int height = 0;
    int strideInBytes = 0;
    int padding = 0;
    int skipMissing = 0;
    unsigned hOversample = 0;
    unsigned vOversample = 0;
    unsigned char* pixels = nullptr;
    void* nodes = nullptr;
};
