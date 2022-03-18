#include "gen.h"

namespace LemonTea
{

PlacementMap::PlacementMap()
{
    this->clear();
};

void PlacementMap::clear()
{
    memset(this->data, false, 1000);
};

bool PlacementMap::get(int8_t x, int8_t y, PieceRotation r)
{
    assert(x >= 0 && x < 10);
    assert(y >= 0 && y < 25);
    assert(r >= 0 && r < 4);
    return this->data[x][y][r];
};

void PlacementMap::set(int8_t x, int8_t y, PieceRotation r, bool value)
{
    assert(x >= 0 && x < 10);
    assert(y >= 0 && y < 25);
    assert(r >= 0 && r < 4);
    this->data[x][y][r] = value;
};

Generator::Generator()
{
    this->queue.reserve(256);
    this->locks.reserve(128);
    this->clear();
};

void Generator::generate(Board& board, PieceType type)
{
    this->clear();

    bool fast_mode = true;
    for (int i = 0; i < 10; ++i) {
        if (64 - std::countl_zero(board[i]) > 16) {
            fast_mode = false;
            break;
        }
    }

    if (fast_mode) {
        int8_t rcount = 4;
        if (type == PIECE_O) {
            rcount = 1;
        }

        for (int8_t r = 0; r < rcount; ++r) {
            for (int8_t x = 0; x < 10; ++x) {
                if (board.is_colliding(x, int8_t(19), type, PieceRotation(r))) {
                    continue;
                }

                Piece location = Piece(x, int8_t(19), type, PieceRotation(r));
                location.move_drop(board);

                this->expand(board, location, true);
                this->lock(board, location);
            }
        }
    }
    else {
        Piece init = Piece(int8_t(4), int8_t(19), type, PIECE_UP);

        if (board.is_colliding(init)) {
            ++init.y;
            if (board.is_colliding(init)) {
                return;
            }
        }

        queue.push_back(init);
        this->queue_map.set(init.x, init.y, init.rotation, true);
    }

    while (!this->queue.empty())
    {
        Piece location = this->queue.back();
        this->queue.pop_back();

        this->expand(board, location, fast_mode);
        this->lock(board, location);
    }
};

void Generator::expand(Board& board, Piece piece, bool fast_mode)
{
    Piece drop = piece;
    drop.move_drop(board);
    if (drop.y != piece.y && !this->queue_map.get(drop.x, drop.y, drop.rotation) && !(fast_mode && board.is_above_stack(drop))) {
        queue.push_back(drop);
        this->queue_map.set(drop.x, drop.y, drop.rotation, true);
    }

    Piece right = piece;
    if (right.move_right(board) && !this->queue_map.get(right.x, right.y, right.rotation) && !(fast_mode && board.is_above_stack(right))) {
        queue.push_back(right);
        this->queue_map.set(right.x, right.y, right.rotation, true);
    }

    Piece left = piece;
    if (left.move_left(board) && !this->queue_map.get(left.x, left.y, left.rotation) && !(fast_mode && board.is_above_stack(left))) {
        queue.push_back(left);
        this->queue_map.set(left.x, left.y, left.rotation, true);
    }

    if (piece.type == PIECE_O) {
        return;
    }

    Piece cw = piece;
    if (cw.move_cw(board) && !this->queue_map.get(cw.x, cw.y, cw.rotation) && !(fast_mode && board.is_above_stack(cw))) {
        queue.push_back(cw);
        this->queue_map.set(cw.x, cw.y, cw.rotation, true);
    }

    Piece ccw = piece;
    if (ccw.move_ccw(board) && !this->queue_map.get(ccw.x, ccw.y, ccw.rotation) && !(fast_mode && board.is_above_stack(ccw))) {
        queue.push_back(ccw);
        this->queue_map.set(ccw.x, ccw.y, ccw.rotation, true);
    }
};

void Generator::lock(Board& board, Piece piece)
{
    piece.move_drop(board);
    if (piece.y >= 20) {
        return;
    }
    piece.normalize();
    if (!this->locks_map.get(piece.x, piece.y, piece.rotation)) {
        this->locks.push_back(piece);
        this->locks_map.set(piece.x, piece.y, piece.rotation, true);
    }
};

void Generator::clear()
{
    this->queue.clear();
    this->locks.clear();
    this->queue_map.clear();
    this->locks_map.clear();
};

};