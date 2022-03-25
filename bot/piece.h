#pragma once

#include "def.h"

namespace LemonTea
{

class Board;

class Piece
{
public:
    int8_t x = 0;
    int8_t y = 0;
    PieceType type = PIECE_NONE;
    PieceRotation rotation = PIECE_UP;
public:
    Piece();
    Piece(int8_t x, int8_t y, PieceType t, PieceRotation r);
public:
    bool operator == (Piece& other);
    bool operator == (const Piece& other);
public:
    bool move_right(Board& board);
    bool move_left(Board& board);
    bool move_cw(Board& board);
    bool move_ccw(Board& board);
    bool move_down(Board& board);
    void move_drop(Board& board);
public:
    void place(Board& board);
public:
    void normalize();
    void mirror();
public:
    Piece get_normalize();
    Piece get_mirror();
};

};