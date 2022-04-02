#include <iostream>
#include <thread>
#include <fstream>

#define OLC_PGE_APPLICATION
#include "app.h"

using namespace std;

int main()
{
    srand((unsigned int)time(NULL));

    // PptSync* ppt = pptsync_new();
    // int frame = 0;
    // while (true)
    // {
    //     if (!pptsync_wait_for_frame(ppt)) {
    //         cout << "smt wrong" << endl;
    //         break;
    //     }
    //     cout << "frame " << frame << endl;
    //     ++frame;
    //     if (frame > 1000) break;
    // }
    // cout << "finished!" << endl;

    app ltapp;
    if (ltapp.Construct(250, 500, 1, 1, false, true, false)) {
		ltapp.Start();
    }

    // cin.get();
    return 0;
};