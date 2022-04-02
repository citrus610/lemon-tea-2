#pragma once

#include "box.h"

class Slider
{
public:
    Box box;
private:
    int value = 0;
    int value_min = 0;
    int value_max = 0;
private:
    bool drag = false;
    bool enable = true;
public:
    Slider();
    Slider(int slider_x, int slider_y, int slider_width, int slider_height, int min, int max);
public:
    void set_position(int slider_x, int slider_y);
    void set_size(int slider_width, int slider_height);
    void set(int slider_x, int slider_y, int slider_width, int slider_height);
public:
    void set_range(int min, int max);
    void set_value(int v);
    void set_enable(bool slider_enable);
public:
    int get_value();
public:
    void update(olc::PixelGameEngine* game, float dt);
};