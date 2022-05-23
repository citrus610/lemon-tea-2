#include "spsa.h"

Spsa::Spsa()
{
    this->generator = std::default_random_engine((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
};

int Spsa::random(int delta)
{
    std::normal_distribution<double> distribution(0.0, (double)delta / 2.0);

    double number = distribution(this->generator);
    return int(number);
};

void Spsa::vary_value(int& base, int& v1, int& v2, int delta)
{
    int r_value = Spsa::random(delta);
    v1 = base + r_value;
    v2 = base - r_value;
};

void Spsa::vary_value_static(int& base, int& v1, int& v2, int delta)
{
    int sign = (rand() % 2) * 2 - 1;
    v1 = base + delta * sign;
    v2 = base - delta * sign;
};

void Spsa::vary_weight(LemonTea::Heuristic& base, LemonTea::Heuristic& v1, LemonTea::Heuristic& v2)
{
    // Set init
    v1 = base;
    v2 = base;

    // Defence
    // VARY_WEIGHT_STATIC_PARAMETER(evaluation.height, 10);
    // VARY_WEIGHT_STATIC_PARAMETER(evaluation.height_10, 10);
    // VARY_WEIGHT_PARAMETER(evaluation.height_15, 100);
    VARY_WEIGHT_STATIC_PARAMETER(evaluation.bumpiness, 10 + (rand() % 6));
    VARY_WEIGHT_STATIC_PARAMETER(evaluation.bumpiness_s, 5);
    // VARY_WEIGHT_PARAMETER(evaluation.row_t, 25);
    VARY_WEIGHT_STATIC_PARAMETER(evaluation.hole_a, 10 + (rand() % 21));
    VARY_WEIGHT_STATIC_PARAMETER(evaluation.hole_b, 10 + (rand() % 21));
    VARY_WEIGHT_STATIC_PARAMETER(evaluation.blocked, 5);
    // VARY_WEIGHT_STATIC_PARAMETER(evaluation.well, 10 + (rand() % 11));
    // VARY_WEIGHT_STATIC_PARAMETER(evaluation.sidewell, 10);
    for (int i = 0; i < 4; ++i) {
        VARY_WEIGHT_STATIC_PARAMETER(evaluation.structure[i], 20 + (rand() % 21));
    }
    VARY_WEIGHT_STATIC_PARAMETER(evaluation.b2b, 20 + (rand() % 6));

    // Attack
    for (int i = 0; i < 4; ++i) {
        VARY_WEIGHT_STATIC_PARAMETER(accumulate.clear[i], 25 + (rand() % 21));
    }
    for (int i = 0; i < 3; ++i) {
        VARY_WEIGHT_STATIC_PARAMETER(accumulate.tspin[i], 10 + (rand() % 21));
    }
    // VARY_WEIGHT_PARAMETER(accumulate.waste_time, 25);
    // VARY_WEIGHT_STATIC_PARAMETER(accumulate.waste_clear, 10 + (rand() % 11));
    VARY_WEIGHT_STATIC_PARAMETER(accumulate.waste_T, 20 + (rand() % 21));
    VARY_WEIGHT_STATIC_PARAMETER(accumulate.b2b, 20 + (rand() % 11));
    VARY_WEIGHT_STATIC_PARAMETER(accumulate.ren, 20 + (rand() % 11));
};

void Spsa::approach_value(int& base, int& v, double ap_v)
{
    double delta_raw = double(v) - double(base);
    double delta = delta_raw * ap_v;
    base += int(std::round(delta));
};

void Spsa::approach_weight(LemonTea::Heuristic& base, LemonTea::Heuristic& v)
{
    // Setting apply factor
    double ap_v = 0.1;

    // Defence
    APPROACH_WEIGHT_PARAMETER(evaluation.height, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.height_10, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.height_15, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.bumpiness, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.bumpiness_s, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.row_t, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.hole_a, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.hole_b, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.blocked, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.well, ap_v);
    APPROACH_WEIGHT_PARAMETER(evaluation.sidewell, ap_v);
    for (int i = 0; i < 4; ++i) {
        APPROACH_WEIGHT_PARAMETER(evaluation.structure[i], ap_v);
    }
    APPROACH_WEIGHT_PARAMETER(evaluation.b2b, ap_v);

    // Attack
    for (int i = 0; i < 4; ++i) {
        APPROACH_WEIGHT_PARAMETER(accumulate.clear[i], ap_v);
    }
    for (int i = 0; i < 3; ++i) {
        APPROACH_WEIGHT_PARAMETER(accumulate.tspin[i], ap_v);
    }
    APPROACH_WEIGHT_PARAMETER(accumulate.waste_clear, ap_v);
    APPROACH_WEIGHT_PARAMETER(accumulate.waste_time, ap_v);
    APPROACH_WEIGHT_PARAMETER(accumulate.waste_T, ap_v);
    APPROACH_WEIGHT_PARAMETER(accumulate.b2b, ap_v);
    APPROACH_WEIGHT_PARAMETER(accumulate.ren, ap_v);
};
