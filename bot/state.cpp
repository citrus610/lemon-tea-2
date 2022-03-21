#include "state.h"

namespace LemonTea
{

State::State()
{
    this->board = Board();
    this->current = PIECE_NONE;
    this->hold = PIECE_NONE;
    this->bag = Bag();
    this->next = 0;
    this->b2b = 0;
    this->ren = 0;
};

State::State(Board board, PieceType current, PieceType hold, bool bag[7], int next, int b2b, int ren)
{
    this->board = board;
    this->hold = hold;
    this->next = next;
    this->b2b = b2b;
    this->ren = ren;
    this->bag = Bag(bag);
};

void State::advance(Piece& placement, std::vector<PieceType>& queue, PieceType fpiece, Lock& lock)
{
    if (fpiece != PIECE_NONE) {
        if (this->current == PIECE_NONE) {
            assert(this->hold != PIECE_NONE);
            if (placement.type != fpiece) {
                assert(placement.type == this->hold);
                this->hold = fpiece;
            }
            this->bag.update(fpiece);
        }
        else {
            assert(this->hold == PIECE_NONE);
            if (placement.type != this->current) {
                assert(placement.type == fpiece);
                this->hold = this->current;
                this->bag.update(this->current);
                this->current = fpiece;
            }
            this->bag.update(this->current);
            this->current = PIECE_NONE;
        }
    }
    else {
        if (placement.type != this->current) {
            bool hold_empty = this->hold == PIECE_NONE;
            this->hold = this->current;
            if (hold_empty) {
                assert(this->next < int(queue.size()));
                assert(placement.type == queue[this->next]);
                this->bag.update(this->current);
                this->current = queue[this->next];
                ++this->next;
            }
        }
        this->bag.update(this->current);
        if (this->next < int(queue.size())) {
            this->current = queue[this->next];
            ++this->next;
        }
        else {
            this->current = PIECE_NONE;
        }
    }

    lock.softdrop = !this->board.is_above_stack(placement);
    bool tspin = this->board.is_tspin(placement);
    placement.place(this->board);
    int line = this->board.clear_line();

    if (line > 0) {
        ++this->ren;
        if (tspin) {
            ++this->b2b;
            lock.type = LockType(int(LOCK_TSPIN_1) + line - 1);
        }
        else {
            if (line == 4) {
                ++this->b2b;
            }
            else {
                this->b2b = 0;
            }
            if (this->board.is_perfect()) {
                lock.type = LOCK_PC;
            }
            else {
                lock.type = LockType(int(LOCK_CLEAR_1) + line - 1);
            }
        }
    }
    else {
        this->ren = 0;
        lock.type = LOCK_NONE;
    }
};

};