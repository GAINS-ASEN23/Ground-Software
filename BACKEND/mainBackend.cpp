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

    // Show the code running
    std::cout << "CWSTATE Running..... \n";

    // Integration Stuff
    double totTime = 60 * 60 * 3;
    double dt = 0.1;

    CWSTATE cwstate;
   
    // Constants
    double r_Moon = 1737447.78/1000;										    // Radius of the Moon [m]

    // Example Specific
    double orbitAlt = 50 + r_Moon;
    Eigen::VectorXd Xn(6);
    Xn << 0, 0, 0, 0, 0, 0;

    std::vector<std::vector<double>> PosVector = cwstate.run_CW_Sim_Moon(totTime, dt, dateEx, orbitAlt, Xn);

    for (size_t j = PosVector.size(); j-- > 0; ) {
        printf("\n %g %g %g;", PosVector.at(j).at(0), PosVector.at(j).at(1), PosVector.at(j).at(2));
    }

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
