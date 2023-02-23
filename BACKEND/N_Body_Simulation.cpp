/* MAIN N-BODY PROBLEM SIMULATION FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This file is for the code pertaining to the N-Body Simulation
*/

#include <iostream>
#include "N_Body_Simulation.h"

/*
*
*	FUNCTION NAME:	NBODYFUNC_SEMSC
*
*	SUMMARY: The first NBODY simulation function for use with the SUN-EARTH-MOON-SPACECRAFT system. This function uses leap-frog integration (AKA Kick-Drift-Kick). 
*
*/
std::vector<std::vector<double>> NBODYSIM::NBODYFUNC_SEMSC(double totTime, double dt, std::string date_0, Eigen::RowVector3d v_vec, Eigen::RowVector3d r_vec)
{
	// Define Vector for return
	std::vector<std::vector<double>> PosVector;

	SPICE spiceObj;

	// Constants
	double G = 6.67e-20;														// Gravitational Constant (km^3/kg-s^2)
	double M_E = 5.97219e24;													// Mass of Earth (Kg)
	double M_S = 1.9891e30;														// Mass of Sun (Kg)
	double M_Mn = 7.34767309e22;												// Mass of Moon (Kg)

	// Simulation Time
	double epochTime = spiceObj.EpochTimeCall(date_0);
	double t_end = epochTime + totTime;

	Eigen::RowVector3d a_vec = get_Accel_SEMSC(epochTime, v_vec, r_vec);

	// Sim Loop
	while (epochTime < t_end) {

		// (1/2) kick
		v_vec += a_vec * (dt / 2.0);

		// Drift
		r_vec += v_vec * dt;

		// Update acceleration
		a_vec = get_Accel_SEMSC(epochTime, v_vec, r_vec);

		// (1/2) kick
		v_vec += a_vec * (dt / 2.0);

		// Add to Position Vector
		PosVector.push_back({ r_vec.coeff(0,0), r_vec.coeff(0,1), r_vec.coeff(0,2) });

		// Increase Time Step
		epochTime += dt;
	}

	return PosVector;
}

/*
*
*	FUNCTION NAME:	get_Accel_SEMSC
*
*	SUMMARY: Get the accelerations of the SEMSC system
*
*/
Eigen::RowVector3d NBODYSIM::get_Accel_SEMSC(double epochTime, Eigen::RowVector3d v_vec, Eigen::RowVector3d r_vec)
{
	SPICE spiceObj;

	// Constants
	double G = 6.67e-20;														// Gravitational Constant (km^3/kg-s^2)
	double M_E = 5.97219e24;													// Mass of Earth (Kg)
	double M_S = 1.9891e30;														// Mass of Sun (Kg)
	double M_Mn = 7.34767309e22;												// Mass of Moon (Kg)

	// Create the Position Arrays of Earth and Sun relative to Moon
	Eigen::RowVector3d PosVectorEarth = spiceObj.SpiceCallIndiv(epochTime, Spice::ObjectID::EARTH, Spice::FrameID::J2000, Spice::ObjectID::MOON, Spice::AbCorrectionID::NONE);
	Eigen::RowVector3d PosVectorSun = spiceObj.SpiceCallIndiv(epochTime, Spice::ObjectID::SUN, Spice::FrameID::J2000, Spice::ObjectID::MOON, Spice::AbCorrectionID::NONE);

	// Create Vectors relative to Spacecraft
	Eigen::RowVector3d R_SC_M = -1.0 * r_vec;
	Eigen::RowVector3d R_SC_E = R_SC_M + PosVectorEarth;
	Eigen::RowVector3d R_SC_S = R_SC_M + PosVectorSun;

	// Pre-Norms of the three individual components
	Eigen::RowVector3d preNorm1 = PosVectorEarth - r_vec;
	Eigen::RowVector3d preNorm2 = PosVectorSun - r_vec;
	Eigen::RowVector3d preNorm3 = R_SC_M - r_vec;

	// Newton's Gravitation - Earth, Sun, Moon affect Spacecraft (km/s^2)
	Eigen::RowVector3d a_vec = G * (((preNorm1)*M_E / pow(preNorm1.norm(), 3)) + ((preNorm2)*M_S / pow(preNorm2.norm(), 3)) + ((preNorm3)*M_Mn / pow(preNorm3.norm(), 3)));

	return a_vec;
}