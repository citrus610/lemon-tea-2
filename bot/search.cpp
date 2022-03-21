#pragma once

#include "search.h"

namespace LemonTea
{

Search::Search()
{
    this->root = Node();
    this->queue.clear();
    this->candidate.clear();
    this->layer[0] = Layer();
    this->layer[1] = Layer();
};

Search::~Search()
{

};

bool Search::init(std::vector<PieceType>& queue)
{
    if (queue.empty()) {
        assert(false);
        return false;
    }

    bool valid_queue = true;
    Bag bag = Bag();
    for (int i = 0; i < int(queue.size()); ++i) {
        if (!bag.data[queue[i]]) {
            valid_queue = false;
            break;
        }
        bag.update(queue[i]);
    }
    if (!valid_queue) {
        assert(false);
        return false;
    }

    this->queue = queue;
    this->clear();
    this->root = Node();
    this->root.state.current = this->queue[0];
    this->root.state.next = 1;
    this->force();
    return true;
};

bool Search::advance(Piece placement, std::vector<PieceType>& next)
{
    // Check if valid placement
    int placement_index = -1;
    for (int i = 0; i < int(this->candidate.size()); ++i) {
        if (this->candidate[i].placement == placement) {
            placement_index = i;
            break;
        }
    }
    if (placement_index == -1) {
        assert(false);
        return false;
    }

    // Check if valid next pieces
    bool valid_next = true;
    Bag bag = this->root.state.bag;
    for (int i = 0; i < int(this->queue.size()); ++i) {
        bag.update(this->queue[i]);
    }
    for (int i = 0; i < int(next.size()); ++i) {
        if (!bag.data[next[i]]) {
            valid_next = false;
            break;
        }
        bag.update(next[i]);
    }
    if (!valid_next) {
        assert(false);
        return false;
    }

    // Update queue
    for (int i = 0; i < this->candidate[placement_index].node.state.next - this->root.state.next; ++i) {
        this->queue.erase(this->queue.begin());
    }
    for (int i = 0; i < int(next.size()); ++i) {
        this->queue.push_back(next[i]);
    }

    // Update root
    this->root = this->candidate[placement_index].node;
    this->root.state.next = 1;
    this->root.score = Score();
    this->root.index = -1;
    this->clear();

    // Force first search
    this->force();

    return true;
};

void Search::reset(Board& board, int b2b, int ren)
{

};

void Search::force()
{
    
};

void Search::clear()
{
    this->candidate.clear();
    this->layer[0].clear();
    this->layer[1].clear();
};

void Search::expand(Node& parent, Layer& layer, int& count)
{

};

void Search::expand_forecast(Node& parent, Layer& layer, int& count)
{

};

void Search::think(int& layer, int& index, int& width, int& count)
{
    
};

void Search::search(int iteration)
{
    int layer = 0;
    int index = 0;
    int width = SEARCH_WIDTH;
    int count = 0;
    for (int i = 0; i < iteration; ++i) {
        this->think(layer, index, width, count);
    }
};

void Search::request(int incomming, Candidate& result)
{
    return Search::pick(this->root, this->candidate, incomming, result);
};

void Search::pick(Node& root, std::vector<Candidate> candidate, int incomming, Candidate& result)
{
    int height[10];
    root.state.board.get_height(height);
    int max_height_center = *std::max_element(height + 3, height + 7);

    std::sort(candidate.data(), candidate.data() + candidate.size(), [&] (Candidate& a, Candidate& b) { return b < a; });
    for (int i = 0; i < int(candidate.size()); ++i) {
        if (max_height_center + incomming - candidate[i].spike <= 20) {
            result = candidate[i];
            return;
        }
    }

    result = Candidate();
};

};