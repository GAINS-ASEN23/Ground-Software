/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include <chrono>
#include "..\Libraries\include\Eigen\Dense"

#include "mainBackend.h"


int main()
{

    // Recording the timestamp at the start of the code
    auto beg = std::chrono::high_resolution_clock::now();

    /* MAIN CODE BEGIN */

    CWSTATE cwstate;
    double dt = 1;
   
    // Constants
    double G = 6.67e-20;														// Gravitational Constant (km^3/kg-s^2)
    double M_Mn = 7.34767309e22;												// Mass of Moon (Kg)
    double r_Moon = 1737447.78/1000;										    // Radius of the Moon [m]

    double n = sqrt((G * M_Mn) / (pow(50 + r_Moon, 3)));					    // Compute the Mean Motion for the CW Equations

    Eigen::MatrixXd F_matrix = cwstate.F_matrix(dt, n);

    std::cout << "Here is the matrix m:\n" << F_matrix << std::endl;

    /* MAIN CODE END */

    // Taking a timestamp after the code is ran
    auto end = std::chrono::high_resolution_clock::now();

    // Subtracting the end timestamp from the beginning
    // And we choose to receive the difference in
    // microseconds
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - beg);

    // Displaying the elapsed time
    std::cout << "Elapsed Time: " << duration.count();

    return 0;
}
