#include "plan.h"

namespace LemonTea
{

PathfinderNode::PathfinderNode()
{
    this->placement = Piece();
    this->move.reserve(32);
    this->move.clear();
    this->time = 0;
};

bool PathfinderNode::attempt(Board& board, MoveType move)
{
    bool success = false;
    Piece previous = this->placement;

    switch (move)
    {
    case MOVE_RIGHT:
        success = this->placement.move_right(board);
        break;
    case MOVE_LEFT:
        success = this->placement.move_left(board);
        break;
    case MOVE_CW:
        success = this->placement.move_cw(board);
        break;
    case MOVE_CCW:
        success = this->placement.move_ccw(board);
        break;
    case MOVE_DOWN:
        this->placement.move_drop(board);
        success = this->placement.y < previous.y;
        break;
    default:
        break;
    }

    if (!success) {
        return false;
    }

    if (!this->move.empty() && this->move.back() == move) {
        this->time += 1;
    }

    this->move.push_back(move);
    this->time += 1;

    if (move == MOVE_DOWN) {
        this->time += std::abs(previous.y - this->placement.y) * 2;
    }

    return true;
};

bool PathfinderNode::operator < (PathfinderNode& other)
{
    // if (this->time == other.time) {
    //     return std::count(this->move.begin(), this->move.end(), MOVE_DOWN) < std::count(other.move.begin(), other.move.end(), MOVE_DOWN);
    // }
    // return this->time < other.time;

    if (std::count(this->move.begin(), this->move.end(), MOVE_DOWN) == std::count(other.move.begin(), other.move.end(), MOVE_DOWN)) {
        return this->time < other.time;
    }
    return std::count(this->move.begin(), this->move.end(), MOVE_DOWN) < std::count(other.move.begin(), other.move.end(), MOVE_DOWN);
};

bool PathfinderNode::operator > (PathfinderNode& other)
{
    // if (this->time == other.time) {
    //     return std::count(this->move.begin(), this->move.end(), MOVE_DOWN) > std::count(other.move.begin(), other.move.end(), MOVE_DOWN);
    // }
    // return this->time > other.time;

    if (std::count(this->move.begin(), this->move.end(), MOVE_DOWN) == std::count(other.move.begin(), other.move.end(), MOVE_DOWN)) {
        return this->time > other.time;
    }
    return std::count(this->move.begin(), this->move.end(), MOVE_DOWN) > std::count(other.move.begin(), other.move.end(), MOVE_DOWN);
};

bool PathfinderNode::operator == (PathfinderNode& other)
{
    return this->time == other.time && std::count(this->move.begin(), this->move.end(), MOVE_DOWN) == std::count(other.move.begin(), other.move.end(), MOVE_DOWN);
};

PathfinderMap::PathfinderMap()
{
    this->clear();
};

bool PathfinderMap::get(Piece placement, PathfinderNode& node)
{
    node = this->data[placement.x][placement.y][placement.rotation];
    return !node.move.empty();
};

bool PathfinderMap::add(Piece placement, PathfinderNode& node)
{
    if (this->data[placement.x][placement.y][placement.rotation].move.empty()) {
        this->data[placement.x][placement.y][placement.rotation] = node;
        return true;
    }

    if (!(node > this->data[placement.x][placement.y][placement.rotation])) {
        this->data[placement.x][placement.y][placement.rotation] = node;
        return true;
    }

    return false;
};

void PathfinderMap::clear()
{
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 25; ++y) {
            for (int r = 0; r < 4; ++r) {
                this->data[x][y][r].placement = Piece(x, y, PIECE_NONE, PieceRotation(r));
                this->data[x][y][r].move.clear();
                this->data[x][y][r].time = 0;
            }
        }
    }
};

void Pathfinder::search(Board board, Piece destination, std::vector<MoveType>& move)
{
    move.clear();

    std::vector<PathfinderNode> queue;
    std::vector<PathfinderNode> locks;
    PathfinderMap map_queue;
    PathfinderMap map_locks;

    PathfinderNode init = PathfinderNode();

    init.placement = Piece(4, 19, destination.type, PIECE_UP);
    if (board.is_colliding(init.placement)) {
        init.placement.y = 20;
        if (board.is_colliding(init.placement)) {
            move.push_back(MOVE_DOWN);
            return;
        }
    }

    queue.push_back(init);
    map_queue.add(init.placement, init);

    while (!queue.empty())
    {
        PathfinderNode node = queue.back();
        queue.pop_back();

        Pathfinder::expand(board, node, queue, map_queue);
        Pathfinder::lock(board, node, locks, map_locks);
    }

    PathfinderNode final;
    map_locks.get(destination.get_normalize(), final);
    move = final.move;

    if (move.empty() || move.back() != MOVE_DOWN) {
        move.push_back(MOVE_DOWN);
    }
};

void Pathfinder::expand(Board board, PathfinderNode& node, std::vector<PathfinderNode>& queue, PathfinderMap& map_queue)
{
    PathfinderNode n_drop = node;
    if (n_drop.attempt(board, MOVE_DOWN)) {
        Pathfinder::add(n_drop, queue, map_queue);
    }

    PathfinderNode n_right = node;
    if (n_right.attempt(board, MOVE_RIGHT)) {
        Pathfinder::add(n_right, queue, map_queue);
    }

    PathfinderNode n_left = node;
    if (n_left.attempt(board, MOVE_LEFT)) {
        Pathfinder::add(n_left, queue, map_queue);
    }

    if (node.placement.type == PIECE_O) {
        return;
    }

    PathfinderNode n_cw = node;
    if (n_cw.attempt(board, MOVE_CW)) {
        Pathfinder::add(n_cw, queue, map_queue);
    }

    PathfinderNode n_ccw = node;
    if (n_ccw.attempt(board, MOVE_CCW)) {
        Pathfinder::add(n_ccw, queue, map_queue);
    }
};

void Pathfinder::lock(Board board, PathfinderNode& node, std::vector<PathfinderNode>& locks, PathfinderMap& map_locks)
{
    node.placement.move_drop(board);
    node.placement.normalize();

    if (!node.move.empty() && (node.move.back() == MOVE_CW || node.move.back() == MOVE_CCW)) {
        node.time -= 1;
    }

    if (node.move.empty() || node.move.back() != MOVE_DOWN) {
        node.move.push_back(MOVE_DOWN);
        node.time += 1;
    }

    if (!map_locks.add(node.placement, node)) {
        return;
    }

    int index = Pathfinder::index(node, locks);

    if (index == -1) {
        locks.push_back(node);
    }
    else if (node < locks[index]) {
        locks[index] = node;
    }
};

void Pathfinder::add(PathfinderNode& node, std::vector<PathfinderNode>& queue, PathfinderMap& map_queue)
{
    if (!map_queue.add(node.placement, node)) {
        return;
    }

    int index = Pathfinder::index(node, queue);

    if (index == -1) {
        queue.push_back(node);
    }
    else if (node < queue[index]) {
        queue[index] = node;
    }
    else if (node == queue[index]) {
        queue.push_back(node);
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