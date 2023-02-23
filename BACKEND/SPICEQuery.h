/* BACKEND SPICE QUERY HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich
* Purpose: This is the header file to query into a SPICE SPK Kernel for wanted data.
*/

#ifndef SPICEQUERY_H
#define SPICEQUERY_H

// C++ Standard Libraries
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

// Eigen Library
#include "..\Libraries\include\Eigen\Dense"

// SPICE Libraries
#include "cppSpice/spice_label_map.hpp"
#include "cppSpice/spice_kernel_set.hpp"
#include "cppSpice/spice_time.hpp"
#include "cppSpice/spice_ephemeris.hpp"  
#include "cppSpice/spice_error.hpp"

// SPICE Class
class SPICE 
{       
	public: 
		// Necessary Functions for SPICE calling
		std::vector<std::vector<double>>SpiceCall(std::vector<std::string> date, Spice::ObjectID Object, Spice::FrameID Frame, Spice::ObjectID Reference, Spice::AbCorrectionID Aberration);	// Function for querying into SPICE SPK file
		Eigen::RowVector3d SpiceCallIndiv(double epochTime, Spice::ObjectID Object, Spice::FrameID Frame, Spice::ObjectID Reference, Spice::AbCorrectionID Aberration);							// Function for querying into SPICE SPK file with Eigen Vector Output
		
		// Examples of SPICE calls
		void printExampleSpiceData();																																							// Example Function of SPICE data
		void printSpiceData(std::vector<std::vector<double>> PosVector);																														// Function to print SPICE data
		
		// Tools for other use
		double EpochTimeCall(std::string date);																																					// Get Epoch time in seconds
};

#endif