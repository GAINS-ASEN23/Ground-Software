/* MAIN BACKEND HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main header file for the Orbital Trajectory Estimation Software backend.
*/
#ifndef MAIN_H
#define MAIN_H

#include "SPICEQuery.h"
#include "N_Body_Simulation.h"

// Example Data
float SC_mass = 10;														// Temporary Spacecraft Mass
std::string dateEx = { "2022 October 01, 13:00:00 PST" };				// Date
Eigen::RowVector3d R_scM{ 1.7633e6, 2.9656e5, 0 };						// Initial Position Relative to Moon
Eigen::RowVector3d V_scM{ -274.68, 1.6323e3, 0 };						// Initial Velocity Relative to Moon

// Integration Stuff
float dt = 0.01;

#endif