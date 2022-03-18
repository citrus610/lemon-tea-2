#pragma once

#include "node.h"

namespace LemonTea
{

constexpr uint32_t TTABLE_DEFAULT_SIZE = 1 << 12;
constexpr uint32_t TTABLE_DEFAULT_BUCKET_SIZE = 8;

struct TEntry
{
    uint32_t hash = 0;
    int accumulate = 0;
};

struct TBucket
{
    TEntry slot[TTABLE_DEFAULT_BUCKET_SIZE] = { TEntry() };
};

class TTable
{
public:
    TBucket* bucket;
    uint32_t size;
public:
    TTable();
    ~TTable();
public:
    void init(uint32_t size = TTABLE_DEFAULT_SIZE);
    void destroy();
    void clear();
public:
    bool get_entry(uint32_t hash, int& accumulate);
    bool add_entry(uint32_t hash, int accumulate);
public:
    double hashful();
};

};