/* MAIN CLOHESSY-WILTSHIRE EQUATIONS SIMULATION HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This file is for the code headers pertaining to the Clohessy-Wiltshire State Dynamics Calculations of the BACKEND
*/

#ifndef CWSTATE_H
#define CWSTATE_H

#include <cmath>
#include "..\Libraries\include\Eigen\Dense"
#include "SPICEQuery.h"

class CWSTATE
{
public:
	// Compute State
	std::vector<std::vector<double>> run_CW_Sim_Moon(double totTime, double dt, std::string date_0, double orbitAlt, Eigen::VectorXd Xn);

	// Individual Matrices
	Eigen::MatrixXd F_matrix(double dt, double n);
	Eigen::MatrixXd G_matrix(double dt, double n, double t1, double t2);
};

#endif
