#pragma once

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
