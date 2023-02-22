/* MAIN N-BODY PROBLEM SIMULATION HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This file is for the code headers pertaining to the N-Body Simulation
*/

#ifndef NBODYSIM_H
#define NBODYSIM_H

#include <cmath>
#include "..\Libraries\include\Eigen\Dense"
#include "SPICEQuery.h"


class NBODYSIM
{
	public:
		// NBODY Functions
		Eigen::RowVector3d NBODYFUNC(float dt, std::string date, Eigen::RowVector3d v_vec_0, Eigen::RowVector3d r_vec_0);

		// Integrators
		Eigen::RowVector3d Integrator_1(float dt);
};

#endif