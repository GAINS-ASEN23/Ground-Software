/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>

#include "mainBackend.h"

int main()
{
    SPICE spiceOBJ;
    
    // Test Function from sub files 
    spiceOBJ.printSpiceData();

    return 0;
}