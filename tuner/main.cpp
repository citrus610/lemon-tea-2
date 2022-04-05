#include <iostream>
#include "tuner.h"

using namespace std;

int main()
{
    srand((unsigned int)time(NULL));

    Tuner tuner;
    tuner.start();
    
    return 0;
}