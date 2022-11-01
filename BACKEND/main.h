/* MAIN BACKEND HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main header file for the Orbital Trajectory Estimation Software backend.
*/
#pragma once

#include <vector>

// Variables
double PosX, PosY, PosZ;
double Sun_PosX, Sun_PosY, Sun_PosZ;

// Create the Position Arrays of Moon and Sun
std::vector<std::vector<double>> PosVectorMoon;
std::vector<std::vector<double>> PosVectorSun;