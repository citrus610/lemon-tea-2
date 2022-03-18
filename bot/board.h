#pragma once

#include "def.h"

namespace LemonTea
{

class Piece;

class Board
{
public:
    uint64_t data[10] = { 0ULL };
public:
    uint64_t& operator [] (int index);
    bool operator == (Board& other);
public:
    void get_height(int height[10]);
    int get_drop_distance(Piece& piece);
    uint64_t get_mask();
public:
    bool is_occupied(const int8_t& x, const int8_t& y);
    bool is_colliding(const int8_t& x, const int8_t& y, const PieceType& type, const PieceRotation& rotation);
    bool is_colliding(Piece& piece);
    bool is_above_stack(Piece& piece);
    bool is_tspin(Piece& piece);
    bool is_perfect();
public:
    int clear_line();
public:
    void print();
};

};