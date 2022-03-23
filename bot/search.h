#pragma once

#include "layer.h"
#include "gen.h"
#include "eval.h"

namespace LemonTea
{

constexpr size_t SEARCH_WIDTH = 200;
constexpr size_t SEARCH_WIDTH_PRUNE = 20;
constexpr size_t SEARCH_WIDTH_FORECAST = 300;

struct Candidate
{
    Node node = Node();
    Lock lock = Lock();
    Piece placement = Piece();
    int visit = 0;
    int spike = 0;
};

class Search
{
public:
    Node root;
    std::vector<PieceType> queue;
    std::vector<Candidate> candidate;
public:
    Layer layer[2];
public:
    Generator generator;
    Evaluator evaluator;
public:
    Search();
    ~Search();
public:
    bool init(std::vector<PieceType>& queue);
    bool advance(Piece placement, std::vector<PieceType>& next);
    void reset(Board& board, int b2b, int ren);
    void clear();
public:
    void force();
    void expand(Node& parent, Layer& layer, int width, int& count);
    void expand_forecast(Node& parent, Layer& layer, int width, int& count);
    void think(int& pre_layer, int& new_layer, int& index, int& width, bool& forecast, int& count, int& depth);
    void search(int iteration, int& count, int& depth);
public:
    void request(int incomming, Candidate& result);
public:
    static void pick(Node& root, std::vector<Candidate> candidate, int incomming, Candidate& result);
};

static bool operator < (Candidate& a, Candidate& b)
{
    if (a.visit == b.visit) {
        return a.spike < b.spike;
    }
    return a.visit < b.visit;
};

};