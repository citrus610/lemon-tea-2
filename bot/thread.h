#pragma once

#include "search.h"
#include "plan.h"

namespace LemonTea
{

struct Plan
{
    Piece placement;
    State root;
    State result;
    std::vector<MoveType> move;
    int node;
    int depth;
    double eval;
};

struct ThreadOption
{
    Heuristic heuristic = DEFAULT_HEURISTIC();
};

struct ThreadAdvance
{
    Piece placement;
    std::vector<PieceType> next;
};

struct ThreadReset
{
    Board board;
    int b2b;
    int ren;
};

class Thread
{
private:
    std::mutex mutex;
    std::atomic_flag flag_running;
    std::atomic_flag flag_advance;
    std::atomic_flag flag_reset;
    std::vector<ThreadAdvance> buffer_advance;
    std::vector<ThreadReset> buffer_reset;
    std::thread* thread;
private:
    Search search;
    bool forecast;
    int pre_layer;
    int new_layer;
    int index;
    int width;
    int count;
    int depth;
public:
    Thread();
    ~Thread();
public:
    bool start(Board board, PieceType hold, std::vector<PieceType> queue, Bag bag, int b2b, int ren, ThreadOption option);
    bool stop();
public:
    bool advance(Piece placement, std::vector<PieceType> next);
    bool reset(Board board, int b2b, int ren);
    bool request(int incomming, Plan& plan);
    bool is_running();
private:
    void clear();
};

};