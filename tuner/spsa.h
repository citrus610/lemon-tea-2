#pragma once

#include "battle.h"

#include <iostream>
#include <string>
#include <random>

class Spsa
{
public:
    Spsa();
private:
    std::default_random_engine generator;
public:
    int random(int delta);
    void vary_value(int& base, int& v1, int& v2, int delta);
    void vary_value_static(int& base, int& v1, int& v2, int delta);
    void vary_weight(LemonTea::Heuristic& base, LemonTea::Heuristic& v1, LemonTea::Heuristic& v2);
    void approach_value(int& base, int& v, double ap_v);
    void approach_weight(LemonTea::Heuristic& base, LemonTea::Heuristic& v);
};

#define VARY_WEIGHT_PARAMETER(p_name, delta) vary_value(base.p_name, v1.p_name, v2.p_name, delta);
#define VARY_WEIGHT_STATIC_PARAMETER(p_name, delta) vary_value_static(base.p_name, v1.p_name, v2.p_name, delta);
#define APPROACH_WEIGHT_PARAMETER(p_name, ap_v) approach_value(base.p_name, v.p_name, ap_v);