#include "bag.h"

namespace LemonTea
{

Bag::Bag()
{
    memset(this->data, true, 7);
};

Bag::Bag(bool init[7])
{
    memcpy(this->data, init, 7);
};

void Bag::update(PieceType next)
{
    assert(this->data[next]);
    this->data[next] = false;
    if (this->data[0] == false &&
        this->data[1] == false &&
        this->data[2] == false &&
        this->data[3] == false &&
        this->data[4] == false &&
        this->data[5] == false &&
        this->data[6] == false) {
        memset(this->data, true, 7);
    }
};

void Bag::deupdate(PieceType next)
{
    if (this->data[0] == true &&
        this->data[1] == true &&
        this->data[2] == true &&
        this->data[3] == true &&
        this->data[4] == true &&
        this->data[5] == true &&
        this->data[6] == true) {
        memset(this->data, false, 7);
    }
    this->data[next] = true;
};

int Bag::size()
{
    int result = this->data[0];
    for (int i = 1; i < 7; ++i) {
        result += this->data[i];
    }
    return result;
};

bool& Bag::operator [] (int index)
{
    return this->data[index];
};

void Bag::print()
{
    using namespace std;

    for (int i = 0; i < 7; ++i) {
        if (this->data[i]) {
            cout << convert_piece_to_str(PieceType(i)) << " ";
        }
    }

    cout << endl;
};

};