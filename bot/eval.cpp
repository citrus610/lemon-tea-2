#include "eval.h"

namespace LemonTea
{

void Evaluator::evaluate(Node& node, Piece placement, Lock lock)
{
    // Reset node's evaluation
    node.score.evaluation = 0;

    // Copy board for evaluation
    Board board = node.state.board;

    // Get height
    int height[10] = { 0 };
    board.get_height(height);

    // Height
    int max_height = *std::max_element(height, height + 10);
    node.score.evaluation += std::max(max_height - 10, 0) * this->heuristic.evaluation.height_10;
    node.score.evaluation += std::max(max_height - 15, 0) * this->heuristic.evaluation.height_15;

    // Donation
    int tspin_structure[4] = { 0 };
    int donation_depth = (node.state.hold == PIECE_T) + (node.state.bag[PIECE_T]) + (node.state.bag.size() <= 3);
    Evaluator::donation(board, height, donation_depth, tspin_structure);

    // Tspin structure
    for (int i = 0; i < 4; ++i) {
        node.score.evaluation += tspin_structure[i] * this->heuristic.evaluation.structure[i];
    }

    // Height
    max_height = *std::max_element(height, height + 10);
    node.score.evaluation += std::max(max_height - 10, 0) * this->heuristic.evaluation.height_10;

    // Well
    int well_position = 0;
    int well_depth = Evaluator::well(board, height, well_position);
    node.score.evaluation += well_depth * this->heuristic.evaluation.well;

    // Side well
    if (well_position == 0 || well_position == 9) {
        node.score.evaluation += well_depth * this->heuristic.evaluation.sidewell;
    }

    // Bumpiness
    int bumpiness[2] = { 0 };
    Evaluator::bumpiness(height, well_position, bumpiness);
    node.score.evaluation += bumpiness[0] * this->heuristic.evaluation.bumpiness;
    node.score.evaluation += bumpiness[1] * this->heuristic.evaluation.bumpiness_s;

    // Row transition
    int transition_row = Evaluator::transition_row(board, height);
    node.score.evaluation += transition_row * this->heuristic.evaluation.row_t;

    // Hole
    int hole[2] = { 0 };
    Evaluator::hole(board, height, height[well_position], hole);
    node.score.evaluation += hole[0] * this->heuristic.evaluation.hole_a;
    node.score.evaluation += hole[1] * this->heuristic.evaluation.hole_b;

    // Blocked hole
    int blocked = Evaluator::blocked(board, height);
    node.score.evaluation += blocked * this->heuristic.evaluation.blocked;

    // B2b
    node.score.evaluation += (node.state.b2b > 0) * this->heuristic.evaluation.b2b;

    // Locks
    switch (lock.type)
    {
    case LOCK_CLEAR_1:
        node.score.accumulate += this->heuristic.accumulate.clear[0];
        break;
    case LOCK_CLEAR_2:
        node.score.accumulate += this->heuristic.accumulate.clear[1];
        break;
    case LOCK_CLEAR_3:
        node.score.accumulate += this->heuristic.accumulate.clear[2];
        break;
    case LOCK_CLEAR_4:
        node.score.accumulate += this->heuristic.accumulate.clear[3];
        break;
    case LOCK_TSPIN_1:
        node.score.accumulate += this->heuristic.accumulate.tspin[0];
        break;
    case LOCK_TSPIN_2:
        node.score.accumulate += this->heuristic.accumulate.tspin[1];
        break;
    case LOCK_TSPIN_3:
        node.score.accumulate += this->heuristic.accumulate.tspin[2];
        break;
    case LOCK_PC:
        node.score.accumulate += this->heuristic.accumulate.pc;
        break;
    default:
        break;
    }

    // Waste clear
    if (lock.type == LOCK_CLEAR_1 ||
        lock.type == LOCK_CLEAR_2 ||
        lock.type == LOCK_CLEAR_3) {
        if (node.state.ren == 1) {
            node.score.accumulate += this->heuristic.accumulate.waste_clear;
        }
    }

    // Waste time
    if (lock.softdrop &&
        lock.type != LOCK_TSPIN_1 &&
        lock.type != LOCK_TSPIN_2 &&
        lock.type != LOCK_TSPIN_3 &&
        lock.type != LOCK_PC) {
        node.score.accumulate += (20 - placement.y) * this->heuristic.accumulate.waste_time;
    }

    // Waste T
    if (placement.type == PIECE_T &&
        lock.type != LOCK_TSPIN_1 &&
        lock.type != LOCK_TSPIN_2 &&
        lock.type != LOCK_TSPIN_3 &&
        lock.type != LOCK_PC) {
        node.score.accumulate += this->heuristic.accumulate.waste_T;
    }

    // B2b
    node.score.accumulate += (node.state.b2b > 1) * this->heuristic.accumulate.b2b;

    // Ren
    int ren = REN_LUT[std::min(node.state.ren, REN_LUT_SIZE - 1)];
    node.score.accumulate += ren * this->heuristic.accumulate.ren;
};

Piece Evaluator::structure(Board& board, int column_height[10])
{
    for (int x = 0; x < 8; ++x) {
        if (column_height[x + 0] > column_height[x + 1] && column_height[x + 0] + 1 < column_height[x + 2]) {
            if (((board[x + 0] >> (column_height[x + 0] - 1)) & 0b111) == 0b001 &&
                ((board[x + 1] >> (column_height[x + 0] - 1)) & 0b111) == 0b000 &&
                ((board[x + 2] >> (column_height[x + 0] - 1)) & 0b111) == 0b101) {
                return Piece(
                    int8_t(x + 1),
                    int8_t(column_height[x + 0]),
                    PIECE_T,
                    PIECE_DOWN
                );
            }
        }
        if (column_height[x + 2] > column_height[x + 1] && column_height[x + 2] + 1 < column_height[x + 0]) {
            if (((board[x + 0] >> (column_height[x + 2] - 1)) & 0b111) == 0b101 &&
                ((board[x + 1] >> (column_height[x + 2] - 1)) & 0b111) == 0b000 &&
                ((board[x + 2] >> (column_height[x + 2] - 1)) & 0b111) == 0b001) {
                return Piece(
                    int8_t(x + 1),
                    int8_t(column_height[x + 2]),
                    PIECE_T,
                    PIECE_DOWN
                );
            }
        }
        if (column_height[x + 1] >= column_height[x + 0] && column_height[x + 1] + 1 < column_height[x + 2]) {
            if (((board[x + 0] >> (column_height[x + 1] - 3)) & 0b11000) == 0b00000 &&
                ((board[x + 1] >> (column_height[x + 1] - 3)) & 0b11110) == 0b00100 &&
                ((board[x + 2] >> (column_height[x + 1] - 3)) & 0b11111) == 0b10000 &&
                (board.is_occupied(x + 1, column_height[x + 1] - 3) ||
                (!board.is_occupied(x + 1, column_height[x + 1] - 3) &&
                board.is_occupied(x + 2, column_height[x + 1] - 4)))) {
                return Piece(
                    int8_t(x + 2),
                    int8_t(column_height[x + 1] - 2),
                    PIECE_T,
                    PIECE_LEFT
                );
            }
        }
        if (column_height[x + 1] >= column_height[x + 2] && column_height[x + 1] + 1 < column_height[x + 0]) {
            if (((board[x + 0] >> (column_height[x + 1] - 3)) & 0b11111) == 0b10000 &&
                ((board[x + 1] >> (column_height[x + 1] - 3)) & 0b11110) == 0b00100 &&
                ((board[x + 2] >> (column_height[x + 1] - 3)) & 0b11000) == 0b00000 &&
                (board.is_occupied(x + 1, column_height[x + 1] - 3) ||
                (!board.is_occupied(x + 1, column_height[x + 1] - 3) &&
                board.is_occupied(x + 0, column_height[x + 1] - 4)))) {
                return Piece(
                    int8_t(x),
                    int8_t(column_height[x + 1] - 2),
                    PIECE_T,
                    PIECE_RIGHT
                );
            }
        }
    }
    return Piece();
};

void Evaluator::bumpiness(int column_height[10], int well_position, int result[2])
{
    int pre_index = 0;
    if (well_position == 0) {
        pre_index = 1;
    }
    for (int i = 1; i < 10; ++i) {
        if (i == well_position) {
            continue;
        }
        int height_different = std::abs(column_height[pre_index] - column_height[i]);
        result[0] += height_different;
        result[1] += height_different * height_different;
        pre_index = i;
    }
};

void Evaluator::hole(Board& board, int column_height[10], int min_height, int result[2])
{
    for (int i = 0; i < 10; ++i) {
        result[0] += column_height[i] - min_height - std::popcount(board[i] >> min_height);
        result[1] += column_height[i] - std::popcount(board[i]);
        
    }
    result[1] -= result[0];
};

void Evaluator::donation(Board& board, int column_height[10], int depth, int tspin_structure[4])
{
    for (int i = 0; i < depth; ++i) {
        Board copy = board;
        Piece quiet_piece = Evaluator::structure(copy, column_height);
        if (quiet_piece.type == PIECE_NONE) break;
        quiet_piece.place(copy);
        int line_clear = copy.clear_line();
        ++tspin_structure[line_clear];
        if (line_clear >= 2) {
            board = copy;
            board.get_height(column_height);
        }
        else {
            break;
        }
    }
};

int Evaluator::well(Board& board, int column_height[10], int& well_position)
{
    well_position = 0;
    for (int i = 1; i < 10; ++i) {
        if (column_height[i] < column_height[well_position]) {
            well_position = i;
        }
    }
    uint64_t mask = ~0b0;
    for (int i = 0; i < 10; ++i) {
        if (i == well_position) continue;
        mask = mask & board[i];
    }
    mask = mask >> column_height[well_position];
    return std::min(std::countr_one(mask), 15);
};

int Evaluator::transition_row(Board& board, int column_height[10])
{
    int result = 0;
    for (int i = 0; i < 9; ++i) {
        uint64_t xor_column = board[i] ^ board[i + 1];
        result += std::popcount(xor_column);
    }
    result += 64 - std::popcount(board[0]);
    result += 64 - std::popcount(board[9]);
    return result;
};

int Evaluator::blocked(Board& board, int column_height[10])
{
    int result = 0;
    for (int i = 0; i < 10; ++i) {
        uint64_t hole_mask = (~board[i]) & ((1ULL << column_height[i]) - 1);
        while (hole_mask != 0)
        {
            int hole_mask_trz = std::countr_zero(hole_mask);
            result += std::min(column_height[i] - hole_mask_trz - 1, 6);
            hole_mask = hole_mask & (~(1ULL << hole_mask_trz));
        }
    }
    return result;
};

int Evaluator::ren_sum(int ren)
{
    int result = 0;
    for (int i = 3; i <= ren; ++i) {
        result += REN_LUT[std::min(i, REN_LUT_SIZE - 1)];
    }
    return result;
};

int Evaluator::spike(State& state, Lock lock)
{
    int line = 0;
    int attack = 0;
    switch (lock.type)
    {
    case LOCK_CLEAR_1:
        line = 1;
        attack = 0;
        break;
    case LOCK_CLEAR_2:
        line = 2;
        attack = 1;
        break;
    case LOCK_CLEAR_3:
        line = 3;
        attack = 2;
        break;
    case LOCK_CLEAR_4:
        line = 4;
        attack = 4;
        break;
    case LOCK_TSPIN_1:
        line = 1;
        attack = 2;
        break;
    case LOCK_TSPIN_2:
        line = 2;
        attack = 4;
        break;
    case LOCK_TSPIN_3:
        line = 3;
        attack = 6;
        break;
    case LOCK_PC:
        line = 0;
        attack = 10;
        break;
    default:
        break;
    }
    if (state.b2b > 1 &&
        (
            lock.type == LOCK_CLEAR_4 ||
            lock.type == LOCK_TSPIN_1 ||
            lock.type == LOCK_TSPIN_2 ||
            lock.type == LOCK_TSPIN_3
            )
        ) {
        ++attack;
    }
    attack += REN_LUT[std::min(state.ren, REN_LUT_SIZE - 1)];

    return line + attack;
};

};