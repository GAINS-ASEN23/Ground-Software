/* BACKEND SPICE QUERY FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This is the file to query into a SPICE SPK Kernel for wanted data.
*/

#include "SPICEQuery.h"
#include "data.h"

using namespace Spice;

std::vector<std::vector<double>>SPICE::SpiceCall(std::vector<std::string> date, Spice::ObjectID Object, Spice::FrameID Frame, Spice::ObjectID Reference, Spice::AbCorrectionID Aberration)
{
	// Define Vector for return
	std::vector<std::vector<double>> PosVector;

	// Create a class which keeps track of loaded kernels
	// kernels will be unloaded once this instance goes out of scope
	KernelSet Kernels{};
	Kernels.LoadAuxillary("Ephemeris/naif0012.tls"); // Load naif0012.tls
	Kernels.LoadEphemeris("Ephemeris/de440.bsp");    // Load de430.bsp

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

	for (size_t i = date.size(); i-- > 0; ) {

		// Convert a date to an epoch time
		// Refer to the Spice str2et_c documentation for a description of
		// valid string inputs
		double EpochTime = Date2Epoch(date.at(i));

		// Calculate the ephemeris of the moon, relative to the centre of the earth in the J2000
		// reference frame
		EphemerisState State = CalcEphemerisState(Inputs, EpochTime);

		// Catch any errors from a failed calculation
		if (State.CalculationSuccess == false)
		{
			puts(GetErrorAndReset().c_str());
		}

		PosVector.push_back({ State.PosX, State.PosY, State.PosZ });
	}

	return PosVector;
}

void SPICE::printSpiceData() 
{
	// Create the Position Arrays of Moon and Sun
	std::vector<std::vector<double>> PosVectorMoon;
	std::vector<std::vector<double>> PosVectorSun;

	PosVectorMoon = SpiceCall(date, Spice::ObjectID::MOON, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);
	PosVectorSun = SpiceCall(date, Spice::ObjectID::SUN, Spice::FrameID::J2000, Spice::ObjectID::EARTH, Spice::AbCorrectionID::NONE);

	std::cout << "MOON Position - J2000 EARTH CENTER \n";

	for (size_t j = PosVectorMoon.size(); j-- > 0; ) {
		printf("\n Main Position (km) = (%g, %g, %g)\n", PosVectorMoon.at(j).at(0), PosVectorMoon.at(j).at(1), PosVectorMoon.at(j).at(2));
	}

	std::cout << "\n SUN Position - J2000 EARTH CENTER \n";

	for (size_t j = PosVectorSun.size(); j-- > 0; ) {
		printf("\n Main Position (km) = (%g, %g, %g)\n", PosVectorSun.at(j).at(0), PosVectorSun.at(j).at(1), PosVectorSun.at(j).at(2));
	}
}