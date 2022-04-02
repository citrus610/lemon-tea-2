#pragma once

#include "box.h"

class Tickbox
{
public:
    Box box;
private:
    int counter_on = 0;
    int counter_off = 0;
private:
    bool on = true;
    bool enable = true;
public:
    Tickbox();
    Tickbox(int tickbox_x, int tickbox_y, int tickbox_height);
public:
    void set_position(int tickbox_x, int tickbox_y);
    void set_size(int tickbox_height);
    void set(int tickbox_x, int tickbox_y, int tickbox_height);
public:
    bool is_on();
    bool is_off();
    bool is_just_on();
    bool is_just_off();
public:
    void set_on(bool tickbox_on);
    void set_enable(bool tickbox_enable);
public:
    void update(olc::PixelGameEngine* game, float dt);
};