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
    this->generator = Generator();
    this->evaluator.heuristic = DEFAULT_HEURISTIC();
};

Search::~Search()
{
    
};

bool Search::init(std::vector<PieceType>& queue)
{
    // Only accept queue with 2 or more pieces
    if (int(queue.size()) < 2) {
        assert(false);
        return false;
    }

    // Check queue's validation
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

    // Set state
    this->queue = queue;
    this->root = Node();
    this->root.state.current = this->queue[0];
    this->root.state.next = 1;

    // Init memory
    this->candidate.reserve(128);
    this->layer[0].init(std::max(SEARCH_WIDTH, SEARCH_WIDTH_FORECAST));
    this->layer[1].init(std::max(SEARCH_WIDTH, SEARCH_WIDTH_FORECAST));

    // Reset generator & evaluator
    this->generator = Generator();

    // Clear memory & force first search
    this->clear();
    this->force();

    return true;
};

bool Search::init(Board& board, PieceType hold, std::vector<PieceType>& queue, Bag& bag, int b2b, int ren)
{
    // Only accept queue with 2 or more pieces
    if (int(queue.size()) < 2) {
        assert(false);
        return false;
    }

    // Set state
    this->queue = queue;
    this->root = Node();
    this->root.state.board = board;
    this->root.state.current = this->queue[0];
    this->root.state.hold = hold;
    this->root.state.bag = bag;
    this->root.state.next = 1;
    this->root.state.b2b = b2b;
    this->root.state.ren = ren;

    // Init memory
    this->candidate.reserve(128);
    this->layer[0].init(std::max(SEARCH_WIDTH, SEARCH_WIDTH_FORECAST));
    this->layer[1].init(std::max(SEARCH_WIDTH, SEARCH_WIDTH_FORECAST));

    // Reset generator & evaluator
    this->generator = Generator();

    // Clear memory & force first search
    this->clear();
    this->force();

    return true;
};

bool Search::advance(Piece placement, std::vector<PieceType>& next)
{
    // Check if valid placement
    int placement_index = -1;
    for (int i = 0; i < int(this->candidate.size()); ++i) {
        if (this->candidate[i].placement.get_normalize() == placement.get_normalize()) {
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

    // Force first search
    this->clear();
    this->force();

    return true;
};

void Search::reset(Board& board, int b2b, int ren)
{
    this->root.state.board = board;
    this->root.state.b2b = b2b;
    this->root.state.ren = ren;
    this->clear();
    this->force();
};

void Search::force()
{
    assert(this->root.state.current != PIECE_NONE);

    PieceType piece[2];
    piece[0] = this->root.state.current;
    piece[1] = this->root.state.hold;
    if (piece[1] == PIECE_NONE && this->root.state.next < int(this->queue.size())) {
        piece[1] = this->queue[this->root.state.next];
    }
    if (piece[0] == piece[1]) {
        piece[1] = PIECE_NONE;
    }

    for (int p = 0; p < 2; ++p) {
        if (piece[p] == PIECE_NONE) {
            continue;
        }

        generator.generate(this->root.state.board, piece[p]);
        for (int i = 0; i < int(generator.locks.size()); ++i) {
            Candidate candidate = Candidate();
            candidate.placement = generator.locks[i];
            candidate.node = this->root;
            candidate.node.index = int(this->candidate.size());

            candidate.node.state.advance(candidate.placement, this->queue, candidate.lock);
            this->evaluator.evaluate(candidate.node, generator.locks[i], candidate.lock);
            candidate.spike = Evaluator::spike(candidate.node.state, candidate.lock);

            this->candidate.push_back(candidate);
            this->layer[0].data.push_back(candidate.node);
        }
    }

    std::sort(this->layer[0].data.begin(), this->layer[0].data.end(), [&] (Node& a, Node& b) { return b < a; });
};

void Search::clear()
{
    this->candidate.clear();
    this->layer[0].clear();
    this->layer[1].clear();
};

void Search::expand(Node& parent, Layer& layer, int width, int& count)
{
    assert(parent.state.current != PIECE_NONE);
    assert((parent.state.hold != PIECE_NONE) + int(this->queue.size()) - parent.state.next > 0);

    // Generate current piece
    generator.generate(parent.state.board, parent.state.current);
    for (int i = 0; i < int(generator.locks.size()); ++i) {
        Node child = parent;
        Lock lock;
        child.state.advance(generator.locks[i], this->queue, lock);
        this->evaluator.evaluate(child, generator.locks[i], lock);
        layer.add(child, width);
    }
    count += int(generator.locks.size());

    // Generate hold piece
    PieceType hold = parent.state.hold;
    if (hold == PIECE_NONE) {
        assert(parent.state.next < int(this->queue.size()));
        hold = this->queue[parent.state.next];
    }
    if (parent.state.current == hold) {
        return;
    }
    generator.generate(parent.state.board, hold);
    for (int i = 0; i < int(generator.locks.size()); ++i) {
        Node child = parent;
        Lock lock;
        child.state.advance(generator.locks[i], this->queue, lock);
        this->evaluator.evaluate(child, generator.locks[i], lock);
        layer.add(child, width);
    }
    count += int(generator.locks.size());
};

void Search::expand_forecast(Node& parent, Layer& layer, int width, int& count)
{
    assert((parent.state.current != PIECE_NONE) + (parent.state.hold != PIECE_NONE) + int(this->queue.size()) - parent.state.next <= 1);

    for (int findex = 0; findex < 7; ++findex) {
        if ((!parent.state.bag[findex]) || (parent.state.current == PieceType(findex))) {
            continue;
        }

        PieceType piece[2];
        piece[0] = PieceType(findex);
        if (parent.state.current != PIECE_NONE) {
            assert(parent.state.hold == PIECE_NONE);
            piece[1] = parent.state.current;
        }
        else {
            assert(parent.state.hold != PIECE_NONE);
            piece[1] = parent.state.hold;
        }
        if (piece[0] == piece[1]) {
            piece[1] = PIECE_NONE;
        }

        for (int p = 0; p < 2; ++p) {
            if (piece[p] == PIECE_NONE) {
                continue;
            }
            generator.generate(parent.state.board, piece[p]);
            for (int i = 0; i < int(generator.locks.size()); ++i) {
                Node child = parent;
                Lock lock;
                child.state.advance(generator.locks[i], PieceType(findex), lock);
                this->evaluator.evaluate(child, generator.locks[i], lock);
                layer.add(child, width);
            }
            count += int(generator.locks.size());
        }
    }
};

void Search::think(int& pre_layer, int& new_layer, int& index, int& width, bool& forecast, int& count, int& depth)
{
    if (index < 0 || index >= this->layer[pre_layer].data.size()) {
        return;
    }

    if (!forecast) {
        this->expand(this->layer[pre_layer].data[index], this->layer[new_layer], width, count);
    }
    else {
        this->expand_forecast(this->layer[pre_layer].data[index], this->layer[new_layer], width, count);
    }

    ++this->candidate[this->layer[pre_layer].data[index].index].visit;
    ++index;
    if (index < int(this->layer[pre_layer].data.size())) {
        return;
    }
    
    index = 0;
    ++depth;
    this->layer[pre_layer].clear();

    if (int(this->layer[new_layer].data.size()) == width) {
        std::sort_heap
        (
            this->layer[new_layer].data.data(), 
            this->layer[new_layer].data.data() + this->layer[new_layer].data.size(), 
            [&] (Node& a, Node& b) { return b < a; }
        );
    }
    else {
        std::sort
        (
            this->layer[new_layer].data.data(), 
            this->layer[new_layer].data.data() + this->layer[new_layer].data.size(), 
            [&] (Node& a, Node& b) { return b < a; }
        );

        int prune = int(this->layer[new_layer].data.size()) * SEARCH_WIDTH_PRUNE / 100;
        for (int i = 0; i < prune; ++i) {
            this->layer[new_layer].data.pop_back();
        }
    }

    if (!forecast) {
        width = SEARCH_WIDTH;
        if (((this->layer[new_layer].data[0].state.current != PIECE_NONE) +
            (this->layer[new_layer].data[0].state.hold != PIECE_NONE) == 1) &&
            int(this->queue.size()) <= this->layer[new_layer].data[0].state.next) {
            width = SEARCH_WIDTH_FORECAST;
            forecast = true;
        }
    }

    pre_layer = new_layer;
    new_layer = (new_layer + 1) & 1;
};

void Search::search(int iteration, int& count, int& depth)
{
    assert(!this->candidate.empty());
    assert(this->candidate.size() == this->layer[0].data.size());

    if (this->candidate.empty()) {
        return;
    }

    bool forecast = false;
    int pre_layer = 0;
    int new_layer = 1;
    int index = 0;
    int width = SEARCH_WIDTH;

    count = int(this->candidate.size());
    depth = 1;

    for (int i = 0; i < iteration; ++i) {
        this->think(pre_layer, new_layer, index, width, forecast, count, depth);
    }
};

void Search::request(int incomming, Candidate& result)
{
    return Search::pick(this->root, this->candidate, incomming, result);
};

void Search::pick(Node& root, std::vector<Candidate> candidate, int incomming, Candidate& result)
{
    result = Candidate();
    result.placement.type = PIECE_NONE;

    if (candidate.empty()) {
        return;
    }

    int height[10];
    root.state.board.get_height(height);
    int max_height_center = *std::max_element(height + 3, height + 7);

    std::sort(candidate.begin(), candidate.end(), [&] (Candidate& a, Candidate& b) { return b < a; });
    for (int i = 0; i < int(candidate.size()); ++i) {
        if (max_height_center + incomming - candidate[i].spike <= 20) {
            result = candidate[i];
            return;
        }
    }
};

};