#include "ttable.h"

namespace LemonTea
{

TTable::TTable()
{
    this->bucket = nullptr;
    this->size = 0;
};

TTable::~TTable()
{
    this->destroy();
};

void TTable::init(uint32_t size)
{
    assert((size & (size - 1)) == 0);
    if (this->bucket != nullptr) {
        assert(false);
        return;
    }
    this->size = size;
    this->bucket = new TBucket[this->size];
    this->clear();
};

void TTable::destroy()
{
    if (this->bucket == nullptr) {
        return;
    }
    delete[] this->bucket;
};

void TTable::clear()
{
    memset(this->bucket, 0, sizeof(TBucket) * this->size);
};

uint32_t TTable::hash(State& state)
{
    char buffer[90] = { 0 };
    memcpy(buffer, state.board.data, 80);
    memcpy(buffer + 80, state.bag.data, 7);
    buffer[87] = state.hold;
    buffer[88] = char(state.b2b & 0b11111111);
    buffer[89] = char(state.ren & 0b11111111);
    return xxhash32((const void*)buffer, 90, 31);
};

bool TTable::get_entry(uint32_t hash, int& accumulate)
{
    uint32_t index = hash & (this->size - 1);
    for (uint32_t i = 0; i < TTABLE_DEFAULT_BUCKET_SIZE; ++i) {
        if (this->bucket[index].slot[i].hash == hash) {
            accumulate = this->bucket[index].slot[i].accumulate;
            return true;
        }
    }
    return false;
};

bool TTable::add_entry(uint32_t hash, int accumulate)
{
    uint32_t index = hash & (this->size - 1);
    int slot_empty = -1;
    int slot_smallest = -1;
    for (uint32_t i = 0; i < TTABLE_DEFAULT_BUCKET_SIZE; ++i) {
        if (this->bucket[index].slot[i].hash == hash) {
            if (this->bucket[index].slot[i].accumulate < accumulate) {
                this->bucket[index].slot[i].accumulate = accumulate;
                return true;
            }
            return false;
        }
        if (this->bucket[index].slot[i].hash == 0 && this->bucket[index].slot[i].accumulate == 0) {
            slot_empty = int(i);
            continue;
        }
        if (slot_smallest == -1 || this->bucket[index].slot[i].accumulate < this->bucket[index].slot[slot_smallest].accumulate) {
            slot_smallest = int(i);
        }
    }
    if (slot_empty != -1) {
        this->bucket[index].slot[slot_empty].hash = hash;
        this->bucket[index].slot[slot_empty].accumulate = accumulate;
        return true;
    }
    if (slot_smallest != -1) {
        this->bucket[index].slot[slot_smallest].hash = hash;
        this->bucket[index].slot[slot_smallest].accumulate = accumulate;
        return true;
    }
    return false;
};

double TTable::hashful()
{
    uint32_t total = 0;
    for (uint32_t i = 0; i < this->size; ++i) {
        for (uint32_t k = 0; k < TTABLE_DEFAULT_BUCKET_SIZE; ++k) {
            if (this->bucket[i].slot[k].hash != 0 || this->bucket[i].slot[k].accumulate != 0) {
                total += 1;
            }
        }
    }
    return double(total) / double(this->size * TTABLE_DEFAULT_BUCKET_SIZE);
};

};