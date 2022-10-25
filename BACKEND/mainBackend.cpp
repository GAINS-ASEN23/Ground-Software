/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include "main.h"
#include "SPICEQuery.h"

int main()
{
    std::string date = "2024 June 10, 13:00:00 PST";

    SPICE spiceOBJ;
    std::tie(PosX, PosY, PosZ) = spiceOBJ.SpiceCall(date, Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);

    printf("\n Main Position (km) = (%g, %g, %g)\n", PosX, PosY, PosZ);

    system("pause");
    return 0;
}