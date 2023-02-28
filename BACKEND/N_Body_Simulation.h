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
		std::vector<std::vector<double>> NBODYFUNC_SEMSC(double totTime, double dt, std::string date_0, Eigen::RowVector3d v_vec_0, Eigen::RowVector3d r_vec_0);
		std::vector<std::vector<double>> NBODYFUNC_MSC(double totTime, double dt, std::string date_0, Eigen::RowVector3d v_vec_0, Eigen::RowVector3d r_vec_0);
		Eigen::RowVector3d get_Accel_SEMSC(double epochTime, Eigen::RowVector3d v_vec, Eigen::RowVector3d r_vec);
		Eigen::RowVector3d get_Accel_MSC(double epochTime, Eigen::RowVector3d v_vec, Eigen::RowVector3d r_vec);

};

#endif