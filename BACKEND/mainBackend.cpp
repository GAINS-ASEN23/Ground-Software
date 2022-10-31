/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>

#include "main.h"
#include "data.h"
#include "SPICEQuery.h"

using namespace std;

int main()
{
    SPICE spiceOBJ;

    for (int i = 0; i <= 60; i++) {
        std::tie(PosX, PosY, PosZ) = spiceOBJ.SpiceCall(date[i], Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);
        
        PosVector.push_back({ PosX, PosY, PosZ});
    }

    for (size_t j = PosVector.size(); j-- > 0; ) {
        printf("\n Main Position (km) = (%g, %g, %g)\n", PosVector.at(j).at(0), PosVector.at(j).at(1), PosVector.at(j).at(2));
    }

    return 0;
}