/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include "..\Libraries\include\Eigen\Dense"

#include "mainBackend.h"

using Eigen::MatrixXd;

int main()
{
    SPICE spiceOBJ;
    
    // Test Function from sub files 
    spiceOBJ.printSpiceData();

    MatrixXd m(2, 2);
    m(0, 0) = 3;
    m(1, 0) = 2.5;
    m(0, 1) = -1;
    m(1, 1) = m(1, 0) + m(0, 1);
    std::cout << m << std::endl;

    return 0;
}
