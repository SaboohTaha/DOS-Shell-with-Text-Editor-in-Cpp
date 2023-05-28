#include <iostream>
#include <conio.h>
#include "DOS_Shell.h"

using namespace std;

// Ignore this function. Used this to print ASCII values of certain keys
int mains()
{
    int c;
    while (true)
    {
        c = _getch();
        cout << c << endl;
    }
}

int main()
{
    DOS_Shell D("V");
    D.Main();
    return 0;
}