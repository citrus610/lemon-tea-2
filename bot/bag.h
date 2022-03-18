#pragma once

#include "def.h"

namespace LemonTea
{

class Bag
{
public:
    bool data[7];
public:
    Bag();
    Bag(bool init[7]);
public:
    void update(PieceType next);
public:
    int size();
public:
    bool& operator [] (int index);
};

};