#include "piece.h"
#include "board.h"

namespace LemonTea
{

Piece::Piece()
{
    this->x = 0;
    this->y = 0;
    this->type = PIECE_NONE;
    this->rotation = PIECE_UP;
};

Piece::Piece(int8_t x, int8_t y, PieceType t, PieceRotation r)
{
    this->x = x;
    this->y = y;
    this->type = t;
    this->rotation = r;
};

bool Piece::operator == (Piece& other)
{
    return (this->x == other.x) && (this->y == other.y) && (this->type == other.type) && (this->rotation == other.rotation);
};

bool Piece::move_right(Board& board)
{
    if (!board.is_colliding(this->x + 1, this->y, this->type, this->rotation)) {
        ++this->x;
        return true;
    }
    return false;
};

bool Piece::move_left(Board& board)
{
    if (!board.is_colliding(this->x - 1, this->y, this->type, this->rotation)) {
        --this->x;
        return true;
    }
    return false;
};

bool Piece::move_cw(Board& board)
{
    assert(this->type != PIECE_O);
    int8_t srs_index = std::min((int8_t)this->type, (int8_t)1);
    int8_t next_rotation = this->rotation + 1;
    if (next_rotation == 4) {
        next_rotation = 0;
    }
    for (int i = 0; i < 5; ++i) {
        int8_t offset_x = SRS_LUT[srs_index][this->rotation][i][0] - SRS_LUT[srs_index][next_rotation][i][0];
        int8_t offset_y = SRS_LUT[srs_index][this->rotation][i][1] - SRS_LUT[srs_index][next_rotation][i][1];
        if (!board.is_colliding(this->x + offset_x, this->y + offset_y, this->type, (PieceRotation)next_rotation)) {
            this->x += offset_x;
            this->y += offset_y;
            this->rotation = (PieceRotation)next_rotation;
            return true;
        }
    }
    return false;
};

bool Piece::move_ccw(Board& board)
{
    assert(this->type != PIECE_O);
    assert(this->type != PIECE_O);
    int8_t srs_index = std::min((int8_t)this->type, (int8_t)1);
    int8_t next_rotation = this->rotation - 1;
    if (next_rotation == -1) {
        next_rotation = 3;
    }
    for (int i = 0; i < 5; ++i) {
        int8_t offset_x = SRS_LUT[srs_index][this->rotation][i][0] - SRS_LUT[srs_index][next_rotation][i][0];
        int8_t offset_y = SRS_LUT[srs_index][this->rotation][i][1] - SRS_LUT[srs_index][next_rotation][i][1];
        if (!board.is_colliding(this->x + offset_x, this->y + offset_y, this->type, (PieceRotation)next_rotation)) {
            this->x += offset_x;
            this->y += offset_y;
            this->rotation = (PieceRotation)next_rotation;
            return true;
        }
    }
    return false;
};

bool Piece::move_down(Board& board)
{
    if (!board.is_colliding(this->x, this->y - 1, this->type, this->rotation)) {
        --this->y;
        return true;
    }
    return false;
};

void Piece::move_drop(Board& board)
{
    this->y -= board.get_drop_distance(*this);
};

void Piece::place(Board& board)
{
    for (int i = 0; i < 4; ++i) {
        int8_t cell_x = PIECE_LUT[this->type][this->rotation][i][0] + this->x;
        int8_t cell_y = PIECE_LUT[this->type][this->rotation][i][1] + this->y;
        board.data[cell_x] |= (1ULL << cell_y);
    }
};

void Piece::normalize()
{
    switch (this->type)
    {
    case PIECE_I:
        switch (this->rotation)
        {
        case PIECE_UP:
            break;
        case PIECE_RIGHT:
            break;
        case PIECE_DOWN:
            this->rotation = PIECE_UP;
            --this->x;
            break;
        case PIECE_LEFT:
            this->rotation = PIECE_RIGHT;
            ++this->y;
            break;
        default:
            break;
        }
        break;
    case PIECE_S:
        switch (rotation)
        {
        case PIECE_UP:
            break;
        case PIECE_RIGHT:
            break;
        case PIECE_DOWN:
            this->rotation = PIECE_UP;
            --this->y;
            break;
        case PIECE_LEFT:
            this->rotation = PIECE_RIGHT;
            --this->x;
            break;
        default:
            break;
        }
        break;
    case PIECE_Z:
        switch (rotation)
        {
        case PIECE_UP:
            break;
        case PIECE_RIGHT:
            break;
        case PIECE_DOWN:
            this->rotation = PIECE_UP;
            --this->y;
            break;
        case PIECE_LEFT:
            this->rotation = PIECE_RIGHT;
            --this->x;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
};

void Piece::mirror()
{
    switch (type)
    {
    case PIECE_I:
        switch (rotation)
        {
        case PIECE_UP:
            rotation = PIECE_DOWN;
            ++x;
            break;
        case PIECE_RIGHT:
            rotation = PIECE_LEFT;
            --y;
            break;
        default:
            break;
        }
        break;
    case PIECE_S:
        switch (rotation)
        {
        case PIECE_UP:
            rotation = PIECE_DOWN;
            ++y;
            break;
        case PIECE_RIGHT:
            rotation = PIECE_LEFT;
            ++x;
            break;
        default:
            break;
        }
        break;
    case PIECE_Z:
        switch (rotation)
        {
        case PIECE_UP:
            rotation = PIECE_DOWN;
            ++y;
            break;
        case PIECE_RIGHT:
            rotation = PIECE_LEFT;
            ++x;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
};

Piece Piece::get_normalize()
{
    Piece copy = *this;
    copy.normalize();
    return copy;
};

Piece Piece::get_mirror()
{
    Piece copy = *this;
    copy.mirror();
    return copy;
};


};