#pragma once

#include "../lib/pge/olcPixelGameEngine.h"

class Box
{
public:
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
public:
    Box();
    Box(int box_x, int box_y, int box_width, int box_height);
public:
    void set_position(int box_x, int box_y);
    void set_size(int box_width, int box_height);
    void set(int box_x, int box_y, int box_width, int box_height);
public:
    bool is_point_in(int point_x, int point_y);
};