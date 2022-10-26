/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include "main.h"
#include "data.h"
#include "SPICEQuery.h"

int main()
{
    size_t dataLength = sizeof(date)/sizeof(date[0]);

    SPICE spiceOBJ;

    for (int i = 0; i <= dataLength; i++) {
        std::tie(PosX, PosY, PosZ) = spiceOBJ.SpiceCall(date[i], Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);
        PosVector[i][1] = PosX;
        PosVector[i][2] = PosY;
        PosVector[i][3] = PosZ;
    }

    /*
    for (int j = 0; j <= dataLength; j++) {
        printf("\n Main Position (km) = (%g, %g, %g)\n", PosVector[j][1], PosVector[j][2], PosVector[j][3]);
    }
    */

    return 0;
}