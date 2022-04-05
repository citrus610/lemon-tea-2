#pragma once

#include "dev.h"
#include "sprt.h"

constexpr int COMPARE_MAX_BATTLE_FRAME = 20000;
constexpr int COMPARE_MAX_BATTLE = 500;

class Compare
{
    std::mutex mutex;
public:
    void save_json(LemonTea::SaveData& save_data, int gen_id);
    void load_json(LemonTea::SaveData& save_data, int gen_id);
public:
    LemonTea::SaveData data;
public:
    void start(LemonTea::Heuristic base, LemonTea::Heuristic w1, LemonTea::Heuristic w2, int total, int gen_id, int thread);
};

