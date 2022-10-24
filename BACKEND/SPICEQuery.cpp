/* BACKEND SPICE QUERY FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This is the file to query into a SPICE SPK Kernel for wanted data.
*/

#include <string>
#include <tuple>
#include "SPICEQuery.h"

using namespace Spice;

std::tuple<double, double, double> SPICE::SpiceCall(std::string Date, Spice::ObjectID Object, Spice::FrameID Frame, Spice::ObjectID Reference, Spice::AbCorrectionID Aberration)
{
    // Create a class which keeps track of loaded kernels    
    // kernels will be unloaded once this instance goes out of scope
    KernelSet Kernels{};
    Kernels.LoadAuxillary("../../BACKEND/Ephemeris/naif0012.tls"); // Load naif0012.tls
    Kernels.LoadEphemeris("../../BACKEND/Ephemeris/de440.bsp");    // Load de430.bsp

    // Display any thrown errors and exit
    if (Kernels.HasFailed() == true)
    {
        for (const auto& Message : Kernels.GetErrorLog())
        {
            puts(Message.data());
        }
    }

    // Define some "constant" inputs to an ephemeris calculation
    EphemerisInputs Inputs = EphemerisInputs{ GetObjectString(Object), GetFrameString(Frame), GetObjectString(Reference), GetAbCorrectionString(Aberration) };

    // Convert a date to an epoch time
    // Refer to the Spice str2et_c documentation for a description of
    // valid string inputs
    double EpochTime = Date2Epoch(Date);

    // Calculate the ephemeris of the moon, relative to the centre of the earth in the J2000
    // reference frame
    EphemerisState State = CalcEphemerisState(Inputs, EpochTime);

    // Catch any errors from a failed calculation
    if (State.CalculationSuccess == false)
    {
        puts(GetErrorAndReset().c_str());
    }

    printf("Position (km) = (%g, %g, %g)\n",
        State.PosX,
        State.PosY,
        State.PosZ);

    printf("Velocity (km/s) = (%g, %g, %g)\n",
        State.VelX,
        State.VelY,
        State.VelZ
    );

    printf("Light Second Delay = %g\n", State.LightTime);

    return {State.PosX, State.PosY, State.PosZ};
}