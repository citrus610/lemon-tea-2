#include "layer.h"

namespace LemonTea
{

Layer::Layer()
{
    this->ttable = TTable();
    this->data.clear();
};

Layer::~Layer()
{

};

void Layer::init(size_t reserve)
{
    this->ttable.init();
    this->data.reserve(reserve);
    this->clear();
};

void Layer::clear()
{
    this->ttable.clear();
    this->data.clear();
};

void Layer::add(Node& node, size_t width)
{
    // Check transposition table
    uint32_t hash = this->ttable.hash(node.state);
    if (!this->ttable.add_entry(hash, node.score.accumulate)) {
        return;
    }

    // Push to data
    if (this->data.size() < width) {
        this->data.push_back(node);
        if (this->data.size() == width) {
            std::make_heap(this->data.data(), this->data.data() + this->data.size(), [&] (Node& a, Node& b) { return b < a; });
        }
        return;
    }
    if (this->data[0] < node) {
        std::pop_heap(this->data.data(), this->data.data() + this->data.size(), [&] (Node& a, Node& b) { return b < a; });
        this->data.back() = node;
        std::push_heap(this->data.data(), this->data.data() + this->data.size(), [&] (Node& a, Node& b) { return b < a; });
    }
};

};