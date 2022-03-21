#pragma once

#include "ttable.h"

namespace LemonTea
{

class Layer
{
public:
    TTable ttable;
    std::vector<Node> data;
public:
    Layer();
    ~Layer();
public:
    void init(size_t reserve);
    void clear();
    void add(Node& node, size_t width);
};

};