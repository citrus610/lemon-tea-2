#include <iostream>
#include <windows.h>

#include "../bot/thread.h"
#include "tetris.h"

using namespace std;

int main()
{
    srand((unsigned int)time(NULL));

    TetrisGame::Tetris game(64, 32, 16);
    game.start();

    // LemonTea::Board board;
    // board[9] = 0b00111111;
    // board[8] = 0b00111111;
    // board[7] = 0b00011111;
    // board[6] = 0b00000111;
    // board[5] = 0b00000001;
    // board[4] = 0b00000000;
    // board[3] = 0b00001101;
    // board[2] = 0b00011111;
    // board[1] = 0b00111111;
    // board[0] = 0b11111111;

    // LemonTea::Generator gen;
    // gen.generate(board, LemonTea::PIECE_T);

    // for (auto placement : gen.locks) {
    //     LemonTea::print_path(board, placement);
    // }

    // std::cin.get();
    return 0;
};