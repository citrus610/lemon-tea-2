#pragma once

#include "piece.h"
#include "board.h"

namespace LemonTea
{

enum InputType
{
    INPUT_RIGHT,
    INPUT_LEFT,
    INPUT_CW,
    INPUT_CCW,
    INPUT_DOWN,
    INPUT_DROP,
    INPUT_DROP_CW,
    INPUT_DROP_CCW,
    INPUT_NONE
};

class PathfinderNode
{
public:
    Piece placement;
    std::vector<MoveType> move;
public:
    int get_time();
    int get_score(Piece destination);
public:
    bool cmp(PathfinderNode& other);
    bool cmp(PathfinderNode& other, Piece destination);
};

class Pathfinder
{
public:
    static void search(Board board, Piece destination, std::vector<MoveType>& move);
    static void expand(Board board, PathfinderNode& node, std::vector<PathfinderNode>& children, bool drop);
    static void expand_onstack(Board board, PieceType type, std::vector<PathfinderNode>& onstack);
    static int index(PathfinderNode& node, std::vector<PathfinderNode>& queue);
};

static void move_to_input(std::vector<MoveType>& move, std::vector<InputType>& input)
{
    input.clear();

    for (int i = 0; i < int(move.size()); ++i) {
        if (i > 0 && move[i] == move[i - 1]) {
            input.push_back(INPUT_NONE);
        }

        switch (move[i])
        {
        case MOVE_RIGHT:
            input.push_back(INPUT_RIGHT);
            break;
        case MOVE_LEFT:
            input.push_back(INPUT_LEFT);
            break;
        case MOVE_CW:
            input.push_back(INPUT_CW);
            break;
        case MOVE_CCW:
            input.push_back(INPUT_CCW);
            break;
        case MOVE_DOWN:
            input.push_back(INPUT_DOWN);
            break;
        default:
            break;
        }
    }

    if (!input.empty() && input.back() == INPUT_DOWN) {
        input.pop_back();
    }
    input.push_back(INPUT_DROP);

    if (input.size() < 2) {
        return;
    }

    if (input[input.size() - 2] == INPUT_CW) {
        input.pop_back();
        input.pop_back();
        input.push_back(INPUT_DROP_CW);
    }
    else if (input[input.size() - 2] == INPUT_CCW)
    {
        input.pop_back();
        input.pop_back();
        input.push_back(INPUT_DROP_CCW);
    }
};

static void print_path(Board board, Piece destination) {
    using namespace std;

    vector<MoveType> move;
    Pathfinder::search(board, destination, move);

    std::string move_str;
    for (int i = 0; i < int(move.size()); ++i) {
        move_str += convert_move_to_str(move[i]) + " ";
    }

    Piece piece = Piece(4, 19, destination.type, PIECE_UP);
    if (board.is_colliding(piece)) {
        piece.y += 1;
    }

    {
        Board copy = board;
        piece.place(copy);
        copy.print();
        cin.get();
        system("cls");
    }
    for (int i = 0; i < int(move.size()); ++i) {
        Board copy = board;
        switch (move[i])
        {
        case MOVE_RIGHT:
            piece.move_right(board);
            break;
        case MOVE_LEFT:
            piece.move_left(board);
            break;
        case MOVE_CW:
            piece.move_cw(board);
            break;
        case MOVE_CCW:
            piece.move_ccw(board);
            break;
        case MOVE_DOWN:
            piece.move_drop(board);
            break;
        default:
            break;
        }
        piece.place(copy);
        copy.print();
        cout << move_str << endl;
        cin.get();
        system("cls");
    }
}

};