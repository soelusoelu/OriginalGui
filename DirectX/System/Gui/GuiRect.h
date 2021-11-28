#pragma once

#include <cassert>

struct GuiRect {
    int id = 0;
    
    //input
    unsigned short w = 0;
    unsigned short h = 0;

    //output
    unsigned short x = 0;
    unsigned short y = 0;
    int wasPacked = 0;
};

struct rpNode {
    unsigned short x = 0;
    unsigned short y = 0;
    rpNode* next = nullptr;
};

struct rpContext {
    int width = 0;
    int height = 0;
    int align = 0;
    int numNodes = 0;
    rpNode* activeHead = nullptr;
    rpNode* freeHead = nullptr;
    rpNode extra[2];

    void initialize(int w, int h, rpNode* nodes, int numNodes) {
        assert(w <= 0xffff && h <= 0xffff);

        int i = 0;
        for (; i < numNodes - 1; ++i) {
            nodes[i].next = &nodes[i + 1];
        }

        nodes[i].next = nullptr;
        freeHead = &nodes[0];
        activeHead = &extra[0];
        width = w;
        height = h;
        this->numNodes = numNodes;
        align = (width + numNodes - 1) / numNodes;

        //ノード0は全幅、ノード1はセンチネル
        extra[0].x = 0;
        extra[0].y = 0;
        extra[0].next = &extra[1];
        extra[1].x = static_cast<unsigned short>(width);
        extra[1].y = 65535;
        extra[1].next = nullptr;
    }
};
