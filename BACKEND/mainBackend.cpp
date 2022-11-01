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
    SPICE spiceOBJ;

    for (int i = 0; i <= 60; i++) {
        std::tie(PosX, PosY, PosZ) = spiceOBJ.SpiceCall(date[i], Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);
        std::tie(Sun_PosX, Sun_PosY, Sun_PosZ) = spiceOBJ.SpiceCall(date[i], Spice::ObjectID::SUN, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);
        
        PosVectorMoon.push_back({ PosX, PosY, PosZ});
        PosVectorSun.push_back({ Sun_PosX, Sun_PosY, Sun_PosZ });
    }

    std::cout << "MOON Position - J2000 EARTH CENTER \n";

    for (size_t j = PosVectorMoon.size(); j-- > 0; ) {
        printf("\n Main Position (km) = (%g, %g, %g)\n", PosVectorMoon.at(j).at(0), PosVectorMoon.at(j).at(1), PosVectorMoon.at(j).at(2));
    }

    std::cout << "\n SUN Position - J2000 EARTH CENTER \n";

    for (size_t j = PosVectorMoon.size(); j-- > 0; ) {
        printf("\n Main Position (km) = (%g, %g, %g)\n", PosVectorSun.at(j).at(0), PosVectorSun.at(j).at(1), PosVectorSun.at(j).at(2));
    }

    return 0;
}