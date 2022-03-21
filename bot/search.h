#pragma once

#include "layer.h"

namespace LemonTea
{

constexpr size_t SEARCH_WIDTH = 200;
constexpr size_t SEARCH_WIDTH_FORECAST = 500;

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
    Search();
    ~Search();
public:
    bool init(std::vector<PieceType>& queue);
    bool advance(Piece placement, std::vector<PieceType>& next);
    void reset(Board& board, int b2b, int ren);
    void clear();
public:
    void force();
    void expand(Node& parent, Layer& layer, int& count);
    void expand_forecast(Node& parent, Layer& layer, int& count);
    void think(int& layer, int& index, int& width, int& count);
    void search(int iteration);
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