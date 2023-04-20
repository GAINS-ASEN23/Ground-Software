/* MAIN CLOHESSY-WILTSHIRE EQUATIONS SIMULATION FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This file is for the code pertaining to the Clohessy-Wiltshire State Dynamics Calculations of the BACKEND
*/

#include <iostream>
#include "CWStateDynamics.h"

/*
*
*	FUNCTION NAME: run_CW_Sim_Moon(float orbitAlt)
*
*	SUMMARY: Loop through and compute the the position vector of the spacecraft using the Clohessy-Wiltshire Equations around Moon orbit.
*
*/
std::vector<std::vector<double>> CWSTATE::run_CW_Sim_Moon(double totTime, double dt, std::string date_0, double orbitAlt, Eigen::VectorXd Xn)
{
    // Define Spice Object for timing
    SPICE spiceObj;

	// Define Vector for return
	std::vector<std::vector<double>> PosVector;

	// Constants
    double G = 6.67e-20;														// Gravitational Constant (km^3/(kg*s^2))
	double M_Mn = 7.34767309e22;												// Mass of Moon (Kg)
    double mu = 4.9048695e12;
    double r_Moon = 1737447.78;													// Radius of the Moon [km]

	double n = sqrt((mu) / (pow((orbitAlt + r_Moon), 3)));					// Compute the Mean Motion for the CW Equations
    std::cout << " --- n = " << n << std::endl;

    // Simulation Time
    double epochTime = spiceObj.EpochTimeCall(date_0);
    double t_end = epochTime + totTime;
    double t1 = 0;
    double t2 = 0 + dt;

    // Acceleration Input Vector of Zeros for now
    Eigen::Vector3d u_matrix(0, 0, 0);

	// Simulation Loop
    while (epochTime < t_end) {

        // Compute forward in time
        Xn = CWSTATE::F_matrix(dt, n) * (Xn);// +CWSTATE::G_matrix(dt, n, t1, t2) * u_matrix;

        //printf("\n %g %g %g;", Xn.coeff(0), Xn.coeff(1), Xn.coeff(2));

        // Add to Position Vector
        PosVector.push_back({ Xn.coeff(0), Xn.coeff(1), Xn.coeff(2) });

        // Increase Time Step
        epochTime += dt;
        t1 += dt;
        t2 += dt;
    }

	return PosVector;
}

/*
*
*	FUNCTION NAME: F_matrix
*
*	SUMMARY: Function to put together the state transition matrix
*
*/
Eigen::MatrixXd CWSTATE::F_matrix(double dt, double n) 
{
    // Initialize the F matrix
	Eigen::MatrixXd F(6,6);

    // Define First Row
    F(0,0) = (4 - (3 * cos(dt * n)));
    F(0,1) = 0;
    F(0,2) = 0;
    F(0,3) = (1 / n) * sin(dt * n);
    F(0,4) = (-2 / n) * (cos(dt * n) - 1);
    F(0,5) = 0;

    // Define Second Row
    F(1,0) = 6 * sin(dt * n) - (6 * dt * n);
    F(1,1) = 1;
    F(1,2) = 0;
    F(1,3) = (2 / n) * (cos(dt * n) - 1);
    F(1,4) = (1 / n) * (4 * sin(dt * n) - (3 * dt * n));
    F(1,5) = 0;

    // Define Third Row
    F(2,0) = 0;
    F(2,1) = 0;
    F(2,2) = cos(dt * n);
    F(2,3) = 0;
    F(2,4) = 0;
    F(2,5) = (1 / n) * sin(dt * n);

    // Define Fourth Row
    F(3,0) = 3 * n * sin(dt * n);
    F(3,1) = 0;
    F(3,2) = 0;
    F(3,3) = cos(dt * n);
    F(3,4) = 2 * sin(dt * n);
    F(3,5) = 0;

    // Define Fifth Row
    F(4,0) = 6 * n * (cos(dt * n) - 1);
    F(4,1) = 0;
    F(4,2) = 0;
    F(4,3) = -2 * sin(dt * n);
    F(4,4) = 4 * cos(dt * n) - 3;
    F(4,5) = 0;

    // Define Sixth Row
    F(5,0) = 0;
    F(5,1) = 0;
    F(5,2) = -n * sin(dt * n);
    F(5,3) = 0;
    F(5,4) = 0;
    F(5,5) = cos(dt * n);

    return F;
}

/*
*
*	FUNCTION NAME: G_matrix
*
*	SUMMARY: Function to put together the input transition matrix
*
*/
Eigen::MatrixXd CWSTATE::G_matrix(double dt, double n, double t1, double t2)
{
    // Initialize the G matrix
    Eigen::MatrixXd G(6, 3);

    // Define First Row
    G(0,1) = (2 / (pow(n, 2))) * pow(sin((dt * n) / 2), 2);
    G(0,2) = (-1 / pow(n, 2)) * ((2 * sin(dt * n)) - (2 * dt * n));
    G(0,3) = 0;

    // Define Second Row
    G(1,1) = (1 / pow(n, 2)) * (2 * sin(dt * n) - 2 * dt * n);
    G(1,2) = ((8 / pow(n, 2)) * pow(sin((dt * n) / 2), 2)) + (3 * t1 * t2) - ((3 / 2) * pow(t1, 2)) - ((3 / 2) * pow(t2, 2));
    G(1,3) = 0;

    // Define Third Row
    G(2,1) = 0;
    G(2,2) = 0;
    G(2,3) = (2 / pow(n, 2)) * pow(sin((dt * n) / 2), 2);

    // Define Fourth Row
    G(3,1) = (1 / n) * sin(dt * n);
    G(3,2) = (4 / n) * pow(sin((dt * n) / 2), 2);
    G(3,3) = 0;

    // Define Fifth Row
    G(4,1) = (-4 / n) * pow((dt * n) / 2, 2);
    G(4,2) = (4 / n) * sin(dt * n) - (3 * dt);
    G(4,3) = 0;

    // Define Sixth Row
    G(5,1) = 0;
    G(5,2) = 0;
    G(5,3) = (1 / n) * sin(dt * n);

    return G;
}