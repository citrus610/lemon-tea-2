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
    bool init(Board& board, PieceType hold, std::vector<PieceType>& queue, Bag& bag, int b2b, int ren);
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

static void bench_search()
{
    Search search;
    std::vector<PieceType> queue = { PIECE_I, PIECE_L, PIECE_O, PIECE_Z, PIECE_J };

    search.init(queue);
    int count = search.candidate.size();
    int depth = 1;

    int64_t time = 0;
    auto time_start = std::chrono::high_resolution_clock::now();
    search.search(3000, count, depth);
    auto time_stop = std::chrono::high_resolution_clock::now();
    time += std::chrono::duration_cast<std::chrono::milliseconds>(time_stop - time_start).count();

    std::cout << "time:  " << int(time) << " ms" << std::endl; 
    std::cout << "node:  " << count << std::endl; 
    std::cout << "depth: " << depth << std::endl; 
    std::cout << "nps:   " << int(int64_t(count) / time) << " knodes/s" << std::endl;
};

};