#pragma once

#include "piece.h"
#include "board.h"

namespace LemonTea
{

class PlacementMap
{
public:
    bool data[10][25][4];
public:
    PlacementMap();
public:
    void clear();
    bool get(int8_t x, int8_t y, PieceRotation r);
    void set(int8_t x, int8_t y, PieceRotation r, bool value);
};

class Generator
{
public:
    std::vector<Piece> queue;
    std::vector<Piece> locks;
public:
    PlacementMap queue_map;
    PlacementMap locks_map;
public:
    Generator();
public:
    void generate(Board& board, PieceType type);
    void expand(Board& board, Piece piece, bool fast_mode);
    void lock(Board& board, Piece piece);
    void clear();
};

static void print_gen(Board& board, PieceType type) {
    using namespace std;

    Generator gen;
    gen.generate(board, type);

    for (int i = 0; i < int(gen.locks.size()); ++i) {
        Board b = board;
        gen.locks[i].place(b);
        b.print();

        cin.get();
        system("cls");
    }
}

static void bench_gen()
{
    using namespace std;

    Generator gen;
    Board board;

    auto bench = [&] (Board b) {
        const int count = 1000000;
        for (int8_t t = 0; t < 7; ++t) {
            int64_t time = 0;
            for (int i = 0; i < count; ++i) {
                auto time_start = chrono::high_resolution_clock::now();
                gen.generate(b, PieceType(t));
                auto time_stop = chrono::high_resolution_clock::now();
                time += chrono::duration_cast<chrono::nanoseconds>(time_stop - time_start).count();
            }
            time = time / count;
            cout << "   piece: " << convert_piece_to_str(PieceType(t)) << "   time: " << time << " ns" << endl;
        }
    };

    board[9] = 0b00111111;
    board[8] = 0b00111111;
    board[7] = 0b00011111;
    board[6] = 0b00000111;
    board[5] = 0b00000001;
    board[4] = 0b00000000;
    board[3] = 0b00001101;
    board[2] = 0b00011111;
    board[1] = 0b00111111;
    board[0] = 0b11111111;

    cout << "BOARD TSPIN" << endl;
    bench(board);

    board[9] = 0b111111111;
    board[8] = 0b111111111;
    board[7] = 0b011111111;
    board[6] = 0b011111111;
    board[5] = 0b000111111;
    board[4] = 0b000100110;
    board[3] = 0b010000001;
    board[2] = 0b011110111;
    board[1] = 0b011111111;
    board[0] = 0b011111111;

    cout << "BOARD DT CANNON" << endl;
    bench(board);
    
    board[9] = 0b000011111111;
    board[8] = 0b000011000000;
    board[7] = 0b110011001100;
    board[6] = 0b110011001100;
    board[5] = 0b110011001100;
    board[4] = 0b110011001100;
    board[3] = 0b110011001100;
    board[2] = 0b110000001100;
    board[1] = 0b110000001100;
    board[0] = 0b111111111100;

    cout << "BOARD TERRIBLE" << endl;
    bench(board);
};

};