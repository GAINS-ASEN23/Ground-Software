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
*	SUMMARY: The first NBODY simulation function for use with the SUN-EARTH-MOON-SPACECRAFT system.
*
*/
Eigen::RowVector3d NBODYFUNC_SEMSC(float dt, std::string date, Eigen::RowVector3d v_vec_0, Eigen::RowVector3d r_vec_0)
{
	SPICE spiceObj;

	// Constants
	float G = 6.67e-20;														// Gravitational Constant (km^3/kg-s^2)
	float M_E = 5.97219e24;													// Mass of Earth (Kg)
	float M_S = 1.9891e30;													// Mass of Sun (Kg)
	float M_Mn = 7.34767309e22;												// Mass of Moon (Kg)

	// Create the Position Arrays of Earth and Sun relative to Moon
	Eigen::RowVector3d PosVectorEarth = spiceObj.SpiceCallIndiv(date, Spice::ObjectID::EARTH, Spice::FrameID::J2000, Spice::ObjectID::MOON, Spice::AbCorrectionID::NONE);
	Eigen::RowVector3d PosVectorSun = spiceObj.SpiceCallIndiv(date, Spice::ObjectID::SUN, Spice::FrameID::J2000, Spice::ObjectID::MOON, Spice::AbCorrectionID::NONE);

	// Create Vectors relative to Spacecraft
	Eigen::RowVector3d R_SC_M = -1.0 * r_vec_0;
	Eigen::RowVector3d R_SC_E = R_SC_M + PosVectorEarth;
	Eigen::RowVector3d R_SC_S = R_SC_M + PosVectorSun;

	// Pre-Norms of the three individual components
	Eigen::RowVector3d preNorm1 = PosVectorEarth - r_vec_0;
	Eigen::RowVector3d preNorm2 = PosVectorSun - r_vec_0;
	Eigen::RowVector3d preNorm3 = R_SC_M - r_vec_0;

	// Newton's Gravitation - Earth, Sun, Moon affect Spacecraft (km/s^2)
	Eigen::RowVector3d a_vec = G * (((preNorm1)*M_E / pow(preNorm1.norm(), 3)) + ((preNorm2)*M_S / pow(preNorm2.norm(), 3)) + ((preNorm3)*M_Mn / pow(preNorm3.norm(), 3)));

	//Integrator_1(dt, a_vec, v_vec_0, r_vec_0);
}

/*
*
*	FUNCTION NAME:	Integrator_1
*
*	SUMMARY: This is the first most basic integrator for the N-Body Model. The basis is that every step is integrated by time through cancellation of units.
*
*/
Eigen::RowVector3d Integrator_1(float dt, Eigen::RowVector3d a_vec_0, Eigen::RowVector3d v_vec_0, Eigen::RowVector3d r_vec_0)
{
	Eigen::RowVector3d V_vec = v_vec_0 + a_vec_0 * dt;
	Eigen::RowVector3d P_vec = r_vec_0 + V_vec * dt;
}