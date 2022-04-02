#pragma once

#include "tetris_player.h"

namespace TetrisGame
{

class TetrisBattle
{
public:
    TetrisBattle();
public:
    std::vector<LemonTea::PieceType> bag;
public:
    TetrisPlayer player_1;
    TetrisPlayer player_2;
public:
    LemonTea::Thread bot_1;
    LemonTea::Thread bot_2;
    LemonTea::Heuristic w_1;
    LemonTea::Heuristic w_2;
    std::vector<LemonTea::MoveType> movement_1;
    std::vector<LemonTea::MoveType> movement_2;
public:
    void init();
    void update();
    void update_player(TetrisPlayer& player, LemonTea::Thread& bot, std::vector<LemonTea::MoveType>& movement);
    void unload();
public:
    bool is_gameover();
    int get_winner();
};

}