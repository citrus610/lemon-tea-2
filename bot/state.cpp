#include "state.h"

namespace LemonTea
{

State::State()
{
    this->board = Board();
    this->hold = PIECE_NONE;
    this->bag = Bag();
    this->next = 0;
    this->b2b = 0;
    this->ren = 0;
};

State::State(Board board, PieceType hold, bool bag[7], int next, int b2b, int ren)
{
    this->board = board;
    this->hold = hold;
    this->next = next;
    this->b2b = b2b;
    this->ren = ren;
    this->bag = Bag(bag);
};

void State::advance(Piece& placement, std::vector<PieceType>& queue, Lock& lock)
{
    if (placement.type != queue[this->next]) {
        bool hold_empty = this->hold == PIECE_NONE;
        this->hold = queue[this->next];
        if (hold_empty) {
            this->bag.update(queue[this->next]);
            ++this->next;
            assert(this->next < int(queue.size()));
            assert(placement.type == queue[this->next]);
        }
    }
    this->bag.update(queue[this->next]);
    ++this->next;

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