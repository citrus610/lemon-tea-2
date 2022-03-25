#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cmath>
#include <chrono>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <algorithm>
#include <bitset>
#include <bit>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "../lib/avec.h"
#include "../lib/hash.h"

namespace LemonTea
{

enum PieceType : int8_t
{
    PIECE_I,
    PIECE_J,
    PIECE_L,
    PIECE_O,
    PIECE_S,
    PIECE_T,
    PIECE_Z,
    PIECE_NONE
};

enum PieceRotation : int8_t
{
    PIECE_UP,
    PIECE_RIGHT,
    PIECE_DOWN,
    PIECE_LEFT
};

enum LockType : int8_t
{
    LOCK_CLEAR_1,
    LOCK_CLEAR_2,
    LOCK_CLEAR_3,
    LOCK_CLEAR_4,
    LOCK_TSPIN_1,
    LOCK_TSPIN_2,
    LOCK_TSPIN_3,
    LOCK_PC,
    LOCK_NONE
};

enum MoveType : int8_t
{
    MOVE_RIGHT,
    MOVE_LEFT,
    MOVE_CW,
    MOVE_CCW,
    MOVE_DOWN
};

constexpr int8_t PIECE_LUT[7][4][4][2] =
{
    {
        {{ -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 }},
        {{ 0, 1 }, { 0, 0 }, { 0, -1 }, { 0, -2 }},
        {{ 1, 0 }, { 0, 0 }, { -1, 0 }, { -2, 0 }},
        {{ 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 }}
    },
    {
        {{ -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 }},
        {{ 0, 1 }, { 0, 0 }, { 0, -1 }, { 1, 1 }},
        {{ 1, 0 }, { 0, 0 }, { -1, 0 }, { 1, -1 }},
        {{ 0, -1 }, { 0, 0 }, { 0, 1 }, { -1, -1 }}
    },
    {
        {{ -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, 1 }},
        {{ 0, 1 }, { 0, 0 }, { 0, -1 }, { 1, -1 }},
        {{ 1, 0 }, { 0, 0 }, { -1, 0 }, { -1, -1 }},
        {{ 0, -1 }, { 0, 0 }, { 0, 1 }, { -1, 1 }}
    },
    {
        {{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 }}, 
        {{ 0, 0 }, { 0, -1 }, { 1, 0 }, { 1, -1 }},
        {{ 0, 0 }, { -1, 0 }, { 0, -1 }, { -1, -1 }},
        {{ 0, 0 }, { 0, 1 }, { -1, 0 }, { -1, 1 }}
    },
    {
        {{ -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 }},
        {{ 0, 1 }, { 0, 0 }, { 1, 0 }, { 1, -1 }},
        {{ 1, 0 }, { 0, 0 }, { 0, -1 }, { -1, -1 }},
        {{ 0, -1 }, { 0, 0 }, { -1, 0 }, { -1, 1 }}
    },
    {
        {{ -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 }},
        {{ 0, 1 }, { 0, 0 }, { 0, -1 }, { 1, 0 }},
        {{ 1, 0 }, { 0, 0 }, { -1, 0 }, { 0, -1 }},
        {{ 0, -1 }, { 0, 0 }, { 0, 1 }, { -1, 0 }}
    },
    {
        {{ -1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 }},
        {{ 1, 1 }, { 1, 0 }, { 0, 0 }, { 0, -1 }},
        {{ 1, -1 }, { 0, -1 }, { 0, 0 }, { -1, 0 }},
        {{ -1, -1 }, { -1, 0 }, { 0, 0 }, { 0, 1 }}
    }
};

constexpr int8_t SRS_LUT[2][4][5][2] =
{
    {
        {{ 0, 0 }, { -1, 0 }, { 2, 0 }, { -1, 0 }, { 2, 0 }},
        {{ -1, 0 }, { 0, 0 }, { 0, 0 }, { 0, 1 }, { 0, -2 }},
        {{ -1, 1 }, { 1, 1 }, { -2, 1 }, { 1, 0 }, { -2, 0 }},
        {{ 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, -1 }, { 0, 2 }},
    },
    {
        {{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }},
        {{ 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 }},
        {{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }},
        {{ 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 }},
    }
};

constexpr int REN_LUT_SIZE = 13;
constexpr int REN_LUT[REN_LUT_SIZE] =
{
    0,
    0,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    4,
    5
};

static char convert_piece_to_str(PieceType piece)
{
    switch (piece)
    {
    case PIECE_I:
        return 'I';
        break;
    case PIECE_J:
        return 'J';
        break;
    case PIECE_L:
        return 'L';
        break;
    case PIECE_O:
        return 'O';
        break;
    case PIECE_S:
        return 'S';
        break;
    case PIECE_T:
        return 'T';
        break;
    case PIECE_Z:
        return 'Z';
        break;
    default:
        return ' ';
        break;
    }
};

};