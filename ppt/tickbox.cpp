#include "tickbox.h"

Tickbox::Tickbox()
{
    this->set(0, 0, 0);
};

Tickbox::Tickbox(int tickbox_x, int tickbox_y, int tickbox_height)
{
    this->set(tickbox_x, tickbox_y, tickbox_height);
};

void Tickbox::set_position(int tickbox_x, int tickbox_y)
{
    this->box.set_position(tickbox_x, tickbox_y);
};

void Tickbox::set_size(int tickbox_height)
{
    this->box.set_size(tickbox_height * 2, tickbox_height);
};

void Tickbox::set(int tickbox_x, int tickbox_y, int tickbox_height)
{
    this->box.set_position(tickbox_x, tickbox_y);
    this->box.set_size(tickbox_height * 2, tickbox_height);
};

bool Tickbox::is_on()
{
    return this->counter_on > 0;
};

bool Tickbox::is_off()
{
    return this->counter_off > 0;
};

bool Tickbox::is_just_on()
{
    return this->counter_on == 1;
};

bool Tickbox::is_just_off()
{
    return this->counter_off == 1;
};

void Tickbox::set_on(bool tickbox_on)
{
    this->on = tickbox_on;
};

void Tickbox::set_enable(bool tickbox_enable)
{
    this->enable = tickbox_enable;
};

void Tickbox::update(olc::PixelGameEngine* game, float dt)
{
    bool is_mouse_touching = this->box.is_point_in(game->GetMouseX(), game->GetMouseY());

    if (this->enable) {
        if (is_mouse_touching) {
            olc::HWButton mouse_state = game->GetMouse(olc::Mouse::LEFT);
            if (mouse_state.bPressed) {
                this->on = !this->on;
            }
        }    

        if (this->on) {
            this->counter_on++;
            this->counter_off = 0;
            this->counter_on = std::min(this->counter_on, 2);
        }
        else {
            this->counter_off++;
            this->counter_on = 0;
            this->counter_off = std::min(this->counter_off, 2);
        }
    }

    olc::Pixel color_main(225, 225, 225, 255);
    olc::Pixel color_sub(64, 64, 64, 255);
    if (this->on) {
        color_sub = olc::Pixel(144, 144, 144, 255);
    }
    if (is_mouse_touching) {
        color_main.r += 16;
        color_main.g += 16;
        color_main.b += 16;
        color_sub.r += 16;
        color_sub.g += 16;
        color_sub.b += 16;
    }
    if (!this->enable) {
        color_main = olc::Pixel(128, 128, 128, 255);
        color_sub = olc::Pixel(48, 48, 48, 255);
    }

    int minus_inside = this->box.h / 8;
    game->FillRect(this->box.x + minus_inside, this->box.y + minus_inside, this->box.w - minus_inside * 2, this->box.h - minus_inside * 2, color_sub);
    game->FillRect(this->box.x + this->on * this->box.h, this->box.y, this->box.h, this->box.h, color_main);
};