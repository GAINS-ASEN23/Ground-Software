/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include "cppSpice/spice_label_map.hpp"
#include "cppSpice/spice_kernel_set.hpp"
#include "cppSpice/spice_time.hpp"
#include "cppSpice/spice_ephemeris.hpp"  
#include "cppSpice/spice_error.hpp"

using namespace Spice;

int main()
{
    // Create a class which keeps track of loaded kernels    
    // kernels will be unloaded once this instance goes out of scope
    KernelSet Kernels{};
    Kernels.LoadAuxillary("C:/Users/djpop/source/repos/groundSoftware/BACKEND/Ephemeris/naif0012.tls"); // Load naif0012.tls
    Kernels.LoadEphemeris("C:/Users/djpop/source/repos/groundSoftware/BACKEND/Ephemeris/de440.bsp");    // Load de430.bsp

    // Display any thrown errors and exit
    if (Kernels.HasFailed() == true)
    {
        for (const auto& Message : Kernels.GetErrorLog())
        {
            puts(Message.data());
            return 1;
        }
    }

    // Display contents
    for (const auto& Meta : Kernels.GetMetadata())
    {
        puts(Meta.second.PrettyString().c_str());
    }

    // Define some "constant" inputs to an ephemeris calculation
    EphemerisInputs Inputs = EphemerisInputs{ GetObjectString(ObjectID::MOON), GetFrameString(FrameID::J2000), GetObjectString(ObjectID::EARTH), GetAbCorrectionString(AbCorrectionID::NONE)};

    // Convert a date to an epoch time
    // Refer to the Spice str2et_c documentation for a description of
    // valid string inputs
    double EpochTime = Date2Epoch("2024 June 10, 13:00:00 PST");

    // Calculate the ephemeris of the moon, relative to the centre of the earth in the J2000
    // reference frame
    EphemerisState LunarState = CalcEphemerisState(Inputs, EpochTime);

    // Catch any errors from a failed calculation
    if (LunarState.CalculationSuccess == false)
    {
        puts(GetErrorAndReset().c_str());
        return 1;
    }

    printf("Lunar Position (km) = (%g, %g, %g)\n",
        LunarState.PosX,
        LunarState.PosY,
        LunarState.PosZ);

    printf("Lunar Velocity (km/s) = (%g, %g, %g)\n",
        LunarState.VelX,
        LunarState.VelY,
        LunarState.VelZ
    );

    printf("Light Second Delay = %g\n", LunarState.LightTime);

    return 0;
}