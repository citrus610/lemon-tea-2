#include "slider.h"

Slider::Slider()
{
    this->value = 0;
    this->set(0, 0, 0, 0);
    this->set_range(0, 0);
};

Slider::Slider(int slider_x, int slider_y, int slider_width, int slider_height, int min, int max)
{
    this->value = 0;
    this->set(slider_x, slider_y, slider_width, slider_height);
    this->set_range(min, max);
};

void Slider::set_position(int slider_x, int slider_y)
{
    this->box.set_position(slider_x, slider_y);
};

void Slider::set_size(int slider_width, int slider_height)
{
    this->box.set_size(slider_width, slider_height);
};

void Slider::set(int slider_x, int slider_y, int slider_width, int slider_height)
{
    this->box.set(slider_x, slider_y, slider_width, slider_height);
};

void Slider::set_range(int min, int max)
{
    if (max <= min) {
        return;
    }
    this->value_min = min;
    this->value_max = max;
    this->set_value(this->value);
};

void Slider::set_value(int v)
{
    this->value = v;
    this->value = std::min(this->value, this->value_max);
    this->value = std::max(this->value, this->value_min);
};

void Slider::set_enable(bool slider_enable)
{
    this->enable = slider_enable;
};

int Slider::get_value()
{
    return this->value;
};

void Slider::update(olc::PixelGameEngine* game, float dt)
{
    olc::HWButton mouse_state = game->GetMouse(olc::Mouse::LEFT);
    int mouse_x = game->GetMouseX();
    int mouse_y = game->GetMouseY();
    bool is_mouse_touching = this->box.is_point_in(mouse_x, mouse_y);

    if (this->enable) {
        if (is_mouse_touching && mouse_state.bPressed) {
            this->drag = true;
        }
        if (!mouse_state.bHeld) {
            this->drag = false;
        }

        if (this->drag) {
            if (mouse_x <= this->box.x) {
                this->value = this->value_min;
            }
            else if (mouse_x >= this->box.x + this->box.w) {
                this->value = this->value_max;
            }
            else {
                double step_size = double(this->box.w) / double(this->value_max - this->value_min);
                double value_mouse_raw = double(mouse_x - this->box.x) / step_size + double(this->value_min);
                this->set_value(int(std::round(value_mouse_raw)));
            }
        }
    }
    else {
        this->drag = false;
    }

    olc::Pixel color_main(225, 225, 225, 255);
    olc::Pixel color_sub(144, 144, 144, 255);
    if (is_mouse_touching || this->drag) {
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

    double step_size = double(this->box.w) / double(this->value_max - this->value_min);
    int knob_x = this->box.x + int(double(this->value - this->value_min) * step_size) - this->box.h / 2;
    knob_x = std::max(knob_x, this->box.x);
    knob_x = std::min(knob_x, this->box.x + this->box.w - this->box.h);
    game->FillRect(knob_x, this->box.y, this->box.h, this->box.h, color_main);
};