// (c) 2004 Mattias Hellström 
// Public Domain, use it or abuse it.
//

#include "MTest.h"
#include <string>
#include "axis.hpp"
#include <iostream>
#include <cstdlib>

std::string testIDtoStr (int et, int dt, int dn, int en)
{
    int value = 0;
    std::string str;
    switch (et)
    {
    case 0:
    {                           // HID
        switch (dt)
        {
        case 0:
            value = getIDKeyboardKey (1000 * dn + en);
            break;
        case 1:
            value = getIDMouseButton (1000 * dn + en);
            break;
        case 2:
            value = getIDMouseAxis (1000 * dn + en);
            break;
        case 3:
            value = getIDJoyButton (dn, en);
            break;
        case 4:
            value = getIDJoyAxis (dn, en);
            break;
        case 5:
            value = getIDJoyAxis (dn, en);
            break;
        case 6:
            value = getIDJoyAxis (dn, en);
            break;
        default:
            value = 0;
        }
    }
        break;
    case 1:                    // replay
        value = et * 100000000 + dt * 100000 + dn + en;
        break;
    case 2:                    // AI Driver
        value = et * 100000000 + dt * 100000 + dn + en;
        break;
    default:
        value = 0;
        break;
    }

    axisIDtoStr (value, str);
    return str;
}



int main (int argc, char **argv)
{
    MTestName = "Test Axix Helper Functions";

    MAssert (testIDtoStr (0, 0, 1, 1) == "Keyboard #1, SDL key code #1");
    MAssert (testIDtoStr (0, 0, 2, 10) == "Keyboard #2, SDL key code #10");
    MAssert (testIDtoStr (0, 0, 3, 100) == "Keyboard #3, SDL key code #100");
    MAssert (testIDtoStr (0, 0, 4, 555) == "Keyboard #4, SDL key code #555");
    MAssert (testIDtoStr (0, 0, 5, 999) == "Keyboard #5, SDL key code #999");
    MAssert (testIDtoStr (0, 0, 55, 1) == "Keyboard #55, SDL key code #1");

    MAssert (testIDtoStr (0, 1, 5, 10) == "Mouse #5, button #10");
    MAssert (testIDtoStr (0, 1, 5, 100) == "Mouse #5, button #100");

    MAssert (testIDtoStr (0, 2, 5, 10) == "Mouse #5, axis #10");
    MAssert (testIDtoStr (0, 3, 5, 10) == "Joystick #5, button #10");
    MAssert (testIDtoStr (0, 4, 5, 10) == "Joystick #5, axis #10");

    MAssert (testIDtoStr (1, 0, 5, 10) == "Replay vehicle #0, axis #15");
    MAssert (testIDtoStr (2, 0, 5, 10) == "AI driver #0, axis #15");

    // default
    MAssert (testIDtoStr (3, 0, 5, 10) == "Keyboard #0, SDL key code #0");

    MTestReport ();
    return MTestErrors;
}
