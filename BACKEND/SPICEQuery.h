/* BACKEND SPICE QUERY HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This is the header file to query into a SPICE SPK Kernel for wanted data.
*/

#pragma once

#include "cppSpice/spice_label_map.hpp"
#include "cppSpice/spice_kernel_set.hpp"
#include "cppSpice/spice_time.hpp"
#include "cppSpice/spice_ephemeris.hpp"  
#include "cppSpice/spice_error.hpp"

class SPICE 
{       
	public:            
		std::tuple<double, double, double> SpiceCall(std::string Date, Spice::ObjectID Object, Spice::FrameID Frame, Spice::ObjectID Reference, Spice::AbCorrectionID Aberration);	// Function for querying into SPICE SPK file
};
