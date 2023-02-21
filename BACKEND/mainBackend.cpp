/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include "..\Libraries\include\Eigen\Dense"

#include "mainBackend.h"

int main()
{
	SPICE spiceObj;
	
	// Create the Position Arrays of Moon and Sun
	Eigen::RowVector3d PosVectorMoon = spiceObj.SpiceCallIndiv(dateEx, Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);

	std::cout << "MAINTEST Main Position (km) = " << PosVectorMoon << std::endl;

    return 0;
}
