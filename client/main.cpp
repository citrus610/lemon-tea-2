#include <iostream>
#include "../bot/thread.h"

int main()
{
    using namespace std;
    using namespace LemonTea;

    Thread thread;

    std::vector<PieceType> queue = { PIECE_I, PIECE_L, PIECE_O, PIECE_Z, PIECE_J, PIECE_T, PIECE_S };
    thread.start(Board(), PIECE_NONE, queue, Bag(), 0, 0);
    auto time_1 = chrono::high_resolution_clock::now();

    Plan plan;

    for (int i = 0; i < 15; ++i) {
        cin.get();
        thread.request(0, plan);
        auto time_2 = chrono::high_resolution_clock::now();
        int64_t time = chrono::duration_cast<chrono::milliseconds>(time_2 - time_1).count();
        int nps = 0;
        if (int(time) != 0) {
            nps = plan.node / int(time);
        }
        cout << "id:   " << i << endl;
        cout << "node: " << plan.node << endl;
        cout << "time: " << int(time) << " ms" << endl;
        cout << "nps:  " << nps << " knodes/s" << endl;

        thread.advance(plan.placement, queue);
        time_1 = chrono::high_resolution_clock::now();
    }

    cin.get();
    return 0;
};