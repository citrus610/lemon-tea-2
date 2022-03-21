#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <bit>
#include <chrono>
#include <iostream>

#if defined(_MSC_VER)
#   define FORCE_INLINE __forceinline
#else
#   define FORCE_INLINE inline __attribute__((always_inline))
#endif

FORCE_INLINE static uint32_t getblock32(const uint32_t* p, int i)
{
    return p[i];
};

FORCE_INLINE static uint32_t fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
};

static uint32_t murmur_hash_3_x86_32(const uint8_t* data, int len, uint32_t seed)
{
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);
    for (int i = -nblocks; i; ++i) {
        uint32_t k1 = getblock32(blocks, i);
        k1 *= c1;
        k1 = std::rotl(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = std::rotl(h1, 13); 
        h1 = h1 * 5 + 0xe6546b64;
    }
    const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);
    uint32_t k1 = 0;
    switch (len & 3)
    {
    case 3:
        k1 ^= tail[2] << 16;
    case 2:
        k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = std::rotl(k1, 15);
        k1 *= c2;
        h1 ^= k1;
    }
    h1 ^= len;
    h1 = fmix32(h1);
    return h1;
};

static uint64_t murmur_hash_2_x64_64(const void* key, int len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t* data = (const uint64_t*)key;
    const uint64_t* end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    }

    const unsigned char* data2 = (const unsigned char*)data;

    switch(len & 7)
    {
    case 7:
        h ^= uint64_t(data2[6]) << 48;
    case 6:
        h ^= uint64_t(data2[5]) << 40;
    case 5:
        h ^= uint64_t(data2[4]) << 32;
    case 4:
        h ^= uint64_t(data2[3]) << 24;
    case 3:
        h ^= uint64_t(data2[2]) << 16;
    case 2:
        h ^= uint64_t(data2[1]) << 8;
    case 1:
        h ^= uint64_t(data2[0]);
        h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
};

static void xxprocess(const void* data, uint32_t* state)
{
    const uint32_t* block = (const uint32_t*)data;
    state[0] = std::rotl(state[0] + block[0] * 2246822519U, 13) * 2654435761U;
    state[1] = std::rotl(state[1] + block[1] * 2246822519U, 13) * 2654435761U;
    state[2] = std::rotl(state[2] + block[2] * 2246822519U, 13) * 2654435761U;
    state[3] = std::rotl(state[3] + block[3] * 2246822519U, 13) * 2654435761U;
};

static uint32_t xxhash32(const void* input, uint32_t length, uint32_t seed)
{
    const uint32_t magic_1 = 2654435761U;
    const uint32_t magic_2 = 2246822519U;
    const uint32_t magic_3 = 3266489917U;
    const uint32_t magic_4 = 668265263U;
    const uint32_t magic_5 = 374761393U;
    const uint32_t max_buffer_size = 16;

    unsigned char buffer[max_buffer_size];
    unsigned int buffer_size = 0;
    uint32_t total_length = 0;

    uint32_t state[4] = {
        seed + magic_1 + magic_2,
        seed + magic_2,
        seed,
        seed - magic_1
    };

    total_length += length;
    const unsigned char* data = (const unsigned char*)input;

    if (length < max_buffer_size) {
        while (length-- > 0) {
            buffer[buffer_size++] = *data++;
        }
        return true;
    }

    const unsigned char* stop = data + length;
    const unsigned char* stop_block = stop - max_buffer_size;

    if (buffer_size > 0) {
        while (buffer_size < max_buffer_size) {
            buffer[buffer_size++] = *data++;
        }
        xxprocess(buffer, state);
    }

    while (data <= stop_block)
    {
        xxprocess(data, state);
        data += 16;
    }

    buffer_size = stop - data;
    for (unsigned int i = 0; i < buffer_size; ++i) {
        buffer[i] = data[i];
    }

    uint32_t result = total_length;

    if (total_length >= max_buffer_size) {
        result += std::rotl(state[0], 1) + std::rotl(state[1], 7) + std::rotl(state[2], 12) + std::rotl(state[3], 18);
    }
    else {
        result += state[2] + magic_5;
    }

    data = buffer;
    stop = data + buffer_size;

    for (; data + 4 <= stop; data += 4) {
        result = std::rotl(result + *(uint32_t*)data * magic_3, 17) * magic_4;
    }

    while (data != stop) {
        result = std::rotl(result + (*data++) * magic_5, 11) * magic_1;
    }

    result ^= result >> 15;
    result *= magic_2;
    result ^= result >> 13;
    result *= magic_3;
    result ^= result >> 16;
    return result;
};

static void bench_hash(const void* input, int length, uint32_t seed)
{
    using namespace std;

    const int count = 100000000;

    uint32_t result_murmur = 0;
    int64_t time_murmur = 0;
    for (int i = 0; i < count; ++i) {
        auto time_murmur_start = chrono::high_resolution_clock::now();
        result_murmur = murmur_hash_3_x86_32((const uint8_t*)input, length, seed);
        auto time_murmur_stop = chrono::high_resolution_clock::now();
        time_murmur += chrono::duration_cast<chrono::nanoseconds>(time_murmur_stop - time_murmur_start).count();
    }
    time_murmur = time_murmur / count;
    cout << "MURMUR 3 HASH 32" << endl;
    cout << "   result: " << result_murmur << endl;
    cout << "   time:   " << time_murmur << " ns" << endl;

    uint32_t result_xxhash = 0;
    int64_t time_xxhash = 0;
    for (int i = 0; i < count; ++i) {
        auto time_xxhash_start = chrono::high_resolution_clock::now();
        result_xxhash = xxhash32(input, uint32_t(length), seed);
        auto time_xxhash_stop = chrono::high_resolution_clock::now();
        time_xxhash += chrono::duration_cast<chrono::nanoseconds>(time_xxhash_stop - time_xxhash_start).count();
    }
    time_xxhash = time_xxhash / count;
    cout << "XXHASH 32" << endl;
    cout << "   result: " << result_xxhash << endl;
    cout << "   time:   " << time_xxhash << " ns" << endl;
};