#pragma once

#include "piece.h"
#include "board.h"
#include "bag.h"

namespace LemonTea
{

struct Lock
{
    LockType type = LOCK_NONE;
    bool softdrop = false;
};

class State
{
public:
    Board board;
    PieceType hold;
    Bag bag;
    int next;
    int b2b;
    int ren;
public:
    State();
    State(Board board, PieceType hold, bool bag[7], int next, int b2b, int ren);
public:
    void advance(Piece& placement, std::vector<PieceType>& queue, Lock& lock);
};

};