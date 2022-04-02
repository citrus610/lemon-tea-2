#include "box.h"

Box::Box()
{
    this->set(0, 0, 0, 0);
};

Box::Box(int box_x, int box_y, int box_width, int box_height)
{
    this->set(box_x, box_y, box_width, box_height);
};

void Box::set_position(int box_x, int box_y)
{
    this->x = box_x;
    this->y = box_y;
};

void Box::set_size(int box_width, int box_height)
{
    this->w = box_width;
    this->h = box_height;
};

void Box::set(int box_x, int box_y, int box_width, int box_height)
{
    this->set_position(box_x, box_y);
    this->set_size(box_width, box_height);
};

bool Box::is_point_in(int point_x, int point_y)
{
    return (point_x >= this->x) && (point_y >= this->y) && (point_x <= this->x + this->w) && (point_y <= this->y + this->h);
};