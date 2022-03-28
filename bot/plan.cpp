#include "plan.h"

namespace LemonTea
{

int PathfinderNode::get_time()
{
    std::vector<InputType> input;
    move_to_input(this->move, input);
    return int(input.size());
};

int PathfinderNode::get_score(Piece destination)
{
    int x2 = (this->placement.x - destination.x) * (this->placement.x - destination.x);
    int y2 = (this->placement.y - destination.y) * (this->placement.y - destination.y);
    int r = std::abs(this->placement.rotation - destination.rotation);
    int r2 = r * r;
    if (r == 3) r2 = 1;
    return x2 + y2 + r2;
};

bool PathfinderNode::cmp(PathfinderNode& other)
{
    if (this->get_time() == other.get_time()) {
        return this->move.size() < other.move.size();
    }
    return this->get_time() < other.get_time();
};

bool PathfinderNode::cmp(PathfinderNode& other, Piece destination)
{
    if (this->get_score(destination) == other.get_score(destination)) {
        return this->cmp(other);
    }
    return this->get_score(destination) == other.get_score(destination);
};

void Pathfinder::search(Board board, Piece destination, std::vector<MoveType>& move)
{
    move.reserve(64);
    move.clear();

    if (destination.type == PIECE_NONE ||
        board.is_colliding(destination) ||
        board.get_drop_distance(destination) > 0) {
        return;
    }

    std::vector<PathfinderNode> queue;
    std::vector<PathfinderNode> found;
    Pathfinder::expand_onstack(board, destination.type, queue);

    for (auto node : queue) {
        if (node.placement.get_normalize() == destination.get_normalize()) {
            move = node.move;
            if (move.back() != MOVE_DOWN) {
                move.push_back(MOVE_DOWN);
            }
            return;
        }
    }

    std::make_heap
    (
        queue.begin(), 
        queue.end(), 
        [&] (PathfinderNode& a, PathfinderNode& b) { return b.cmp(a, destination); }
    );

    while (!queue.empty())
    {
        PathfinderNode node = queue[0];
        std::pop_heap
        (
            queue.begin(), 
            queue.end(), 
            [&] (PathfinderNode& a, PathfinderNode& b) { return b.cmp(a, destination); }
        );
        queue.pop_back();

        if (node.placement.get_normalize() == destination.get_normalize()) {
            move = node.move;
            if (move.back() != MOVE_DOWN) {
                move.push_back(MOVE_DOWN);
            }
            return;
        }

        std::vector<PathfinderNode> children;
        Pathfinder::expand(board, node, children, true);

        for (auto child : children) {
            int index_queue = Pathfinder::index(child, queue);
            int index_found = Pathfinder::index(child, found);

            if (index_queue == -1 && index_found == -1) {
                queue.push_back(child);
                std::push_heap
                (
                    queue.begin(), 
                    queue.end(), 
                    [&] (PathfinderNode& a, PathfinderNode& b) { return b.cmp(a, destination); }
                );
            }

            if (index_queue == -1 && index_found != -1) {
                if (child.cmp(found[index_found], destination)) {
                    found[index_found] = child;
                    queue.push_back(child);
                    std::push_heap
                    (
                        queue.begin(), 
                        queue.end(), 
                        [&] (PathfinderNode& a, PathfinderNode& b) { return b.cmp(a, destination); }
                    );
                }
            }

            if (index_queue != -1 && index_found == -1) {
                if (child.cmp(queue[index_queue], destination)) {
                    queue[index_queue] = child;
                    std::make_heap
                    (
                        queue.begin(), 
                        queue.end(), 
                        [&] (PathfinderNode& a, PathfinderNode& b) { return b.cmp(a, destination); }
                    );
                }
            }

            if (index_queue != -1 && index_found != -1) {
                if (child.cmp(found[index_found], destination)) {
                    found[index_found] = child;
                }
                if (child.cmp(queue[index_queue], destination)) {
                    queue[index_queue] = child;
                    std::make_heap
                    (
                        queue.begin(), 
                        queue.end(), 
                        [&] (PathfinderNode& a, PathfinderNode& b) { return b.cmp(a, destination); }
                    );
                }
            }
        }

        found.push_back(node);
    }
};

void Pathfinder::expand(Board board, PathfinderNode& node, std::vector<PathfinderNode>& children, bool drop)
{
    if (drop) {
        PathfinderNode n_drop = node;
        n_drop.placement.move_drop(board);
        if (n_drop.placement.y < node.placement.y) {
            n_drop.move.push_back(MOVE_DOWN);
            children.push_back(n_drop);
        }
    }

    PathfinderNode n_right = node;
    if (n_right.placement.move_right(board)) {
        n_right.move.push_back(MOVE_RIGHT);
        children.push_back(n_right);
    }

    PathfinderNode n_left = node;
    if (n_left.placement.move_left(board)) {
        n_left.move.push_back(MOVE_LEFT);
        children.push_back(n_left);
    }

    if (node.placement.type == PIECE_O) {
        return;
    }

    PathfinderNode n_cw = node;
    if (n_cw.placement.move_cw(board)) {
        n_cw.move.push_back(MOVE_CW);
        children.push_back(n_cw);
    }

    PathfinderNode n_ccw = node;
    if (n_ccw.placement.move_ccw(board)) {
        n_ccw.move.push_back(MOVE_CCW);
        children.push_back(n_ccw);
    }
};

void Pathfinder::expand_onstack(Board board, PieceType type, std::vector<PathfinderNode>& onstack)
{
    onstack.reserve(64);
    onstack.clear();

    std::vector<PathfinderNode> open;
    std::vector<PathfinderNode> close;
    open.reserve(128);
    close.reserve(128);

    PathfinderNode init;
    init.placement = Piece(int8_t(4), int8_t(19), type, PIECE_UP);
    if (board.is_colliding(init.placement)) {
        init.placement.y = 20;
        if (board.is_colliding(init.placement)) {
            return;
        }
    }

    open.push_back(init);
    init.placement.move_drop(board);
    init.move.push_back(MOVE_DOWN);
    onstack.push_back(init);

    while (!open.empty())
    {
        PathfinderNode node = open[0];
        open[0] = open.back();
        open.pop_back();

        std::vector<PathfinderNode> children;
        children.clear();
        Pathfinder::expand(board, node, children, false);

        for (auto child : children) {
            int index_open = Pathfinder::index(child, open);
            int index_close = Pathfinder::index(child, close);

            if (index_open == -1 && index_close == -1) {
                open.push_back(child);
            }

            if (index_open == -1 && index_close != -1) {
                if (!(close[index_close].cmp(child))) {
                    close[index_close] = child;
                    open.push_back(child);
                }
            }

            if (index_open != -1 && index_close == -1) {
                if (child.cmp(open[index_open])) {
                    open[index_open] = child;
                }
                else if (child.get_time() == open[index_open].get_time() && child.move.size() == open[index_open].move.size()) {
                    open.push_back(child);
                }
            }

            if (index_open != -1 && index_close != -1) {
                if (!(close[index_close].cmp(child))) {
                    close[index_close] = child;
                }
                if (child.cmp(open[index_open])) {
                    open[index_open] = child;
                }
                if (child.get_time() == open[index_open].get_time() && child.move.size() == open[index_open].move.size()) {
                    open.push_back(child);
                }
            }

            child.placement.move_drop(board);
            child.move.push_back(MOVE_DOWN);

            int index = Pathfinder::index(child, onstack);
            if (index == -1) {
                onstack.push_back(child);
            }
            else {
                if (child.cmp(onstack[index])) {
                    onstack[index] = child;
                }
            }
        }

        close.push_back(node);
    }
};

int Pathfinder::index(PathfinderNode& node, std::vector<PathfinderNode>& queue)
{
    for (int i = 0; i < int(queue.size()); ++i) {
        if (node.placement == queue[i].placement) {
            return i;
        }
    }
    return -1;
};

};