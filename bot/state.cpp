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

void State::advance(Piece& placement, std::vector<PieceType>& queue, Lock& lock)
{
    if (placement.type != this->current) {
        PieceType previous_hold = this->hold;
        this->hold = this->current;
        if (previous_hold == PIECE_NONE) {
            assert(this->next < int(queue.size()));
            assert(placement.type == queue[this->next]);
            this->bag.update(this->current);
            this->current = queue[this->next];
            ++this->next;
        }
        else {
            assert(placement.type == previous_hold);
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

    this->lock(placement, lock);
};

void State::advance(Piece& placement, PieceType fpiece, Lock& lock)
{
    assert(fpiece != PIECE_NONE);

    PieceType next_piece = PIECE_NONE;
    if (this->current == PIECE_NONE) {
        assert(this->hold != PIECE_NONE);
        this->current = fpiece;
    }
    else {
        assert(this->hold == PIECE_NONE);
        next_piece = fpiece;
    }

    if (placement.type != this->current) {
        PieceType previous_hold = this->hold;
        this->hold = this->current;
        if (previous_hold == PIECE_NONE) {
            assert(placement.type == next_piece);
            this->bag.update(this->current);
            this->current = next_piece;
            next_piece = PIECE_NONE;
        }
        else {
            assert(placement.type == previous_hold);
        }
    }
    this->bag.update(this->current);
    this->current = next_piece;

    this->lock(placement, lock);
};

void State::lock(Piece& placement, Lock& lock)
{
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

void State::print()
{
    using namespace std;

    cout << "Board:" << endl;
    this->board.print();
    cout << "Current: " << convert_piece_to_str(this->current) << endl;
    cout << "Hold:    " << convert_piece_to_str(this->hold) << endl;
    cout << "Bag:     ";
    this->bag.print();
    cout << "Next:    " << this->next << endl;
    cout << "B2b:     " << this->b2b << endl;
    cout << "Ren:     " << this->ren << endl;
};

};