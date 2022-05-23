#pragma once

#include "node.h"

namespace LemonTea
{

struct HeuristicAccumulate
{
    int clear[4] = { 0, 0, 0, 0 };
    int tspin[3] = { 0, 0, 0 };
    int pc = 0;
    int waste_clear = 0;
    int waste_time = 0;
    int waste_T = 0;
    int b2b = 0;
    int ren = 0;
};

struct HeuristicEvaluation
{
    int height = 0;
    int height_10 = 0;
    int height_15 = 0;
    int bumpiness = 0;
    int bumpiness_s = 0;
    int row_t = 0;
    int hole_a = 0;
    int hole_b = 0;
    int blocked = 0;
    int well = 0;
    int sidewell = 0;
    int structure[4] = { 0, 0, 0, 0 };
    int b2b = 0;
};

struct Heuristic
{
    HeuristicAccumulate accumulate = HeuristicAccumulate();
    HeuristicEvaluation evaluation = HeuristicEvaluation();
};

class Evaluator
{
public:
    Heuristic heuristic;
public:
    void evaluate(Node& node, Piece placement, Lock lock);
public:
    static Piece structure(Board& board, int column_height[10]);
    static void bumpiness(int column_height[10], int well_position, int result[2]);
    static void hole(Board& board, int column_height[10], int min_height, int result[2]);
    static void donation(Board& board, int column_height[10], int depth, int tspin_structure[4]);
    static int well(Board& board, int column_height[10], int& well_position);
    static int transition_row(Board& board, int column_height[10]);
    static int blocked(Board& board, int column_height[10]);
    static int ren_sum(int ren);
    static int spike(State& state, Lock lock);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.clear[0] = -309;
    result.accumulate.clear[1] = -228;
    result.accumulate.clear[2] = -146;
    result.accumulate.clear[3] = 290;
    result.accumulate.tspin[0] = 48;
    result.accumulate.tspin[1] = 402;
    result.accumulate.tspin[2] = 726;
    result.accumulate.pc = 10000;
    result.accumulate.waste_clear = -232;
    result.accumulate.waste_time = -17;
    result.accumulate.waste_T = -161;
    result.accumulate.b2b = 184;
    result.accumulate.ren = 140;

    result.evaluation.height = -40;
    result.evaluation.height_10 = -150;
    result.evaluation.height_15 = -600;
    result.evaluation.bumpiness = -8;
    result.evaluation.bumpiness_s = -12;
    result.evaluation.row_t = -20;
    result.evaluation.hole_a = -564;
    result.evaluation.hole_b = -467;
    result.evaluation.blocked = -21;
    result.evaluation.well = 58;
    result.evaluation.sidewell = -22;
    result.evaluation.structure[0] = 99;
    result.evaluation.structure[1] = 150;
    result.evaluation.structure[2] = 253;
    result.evaluation.structure[3] = 513;
    result.evaluation.b2b = 153;

    return result;
};

};