/* MAIN BACKEND HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main header file for the Orbital Trajectory Estimation Software backend.
*/
#ifndef MAIN_H
#define MAIN_H

#include "SPICEQuery.h"
#include "N_Body_Simulation.h"
#include "comms.h"
//#include "ccsds.h"

//#include "boost/array.hpp"
//#include "boost/asio.hpp"
#include <ctime>
//#include <boost/bind.hpp>
//#include <boost/bind/bind.hpp>

// Example Data
std::string dateEx = { "2022 October 01, 13:00:00 PST" };				// Date
Eigen::RowVector3d R_scM{ 1.7633e6/1000, 2.9656e5/1000, 0 };						// Initial Position Relative to Moon
Eigen::RowVector3d V_scM{ -274.68/1000, 1.6323e3/1000, 0 };						// Initial Velocity Relative to Moon

#endif