#include <iostream>
#include "Galois/galois.h"

using namespace std;

int main()
{
    unsigned int x, y;
    x = 150;
    y = 158;

    // unsigned int z = galois_single_multiply(x, y, 8);
    unsigned int z = galois_single_divide(45, 34, 8);

    cout << z << endl;

    return 0;
}
