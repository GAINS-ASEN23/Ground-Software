#pragma once
/* BACKEND SPP Packaging HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Brian Trybus
* Purpose: This is the header file to Format output data for space packet.
*/

#ifndef SpacePacketProtocol_H
#define SpacePacketProtocol_H

// Includes


class SpacePacket { // Space Packet Class will have headers and key data.
private:
	long primaryHeader; // 32 bits for all the header information

	int packetLength; // 16 bits Number of octets in Data feild -1

	// Unsure of what is best way to do our full data first thought is pointer but want to check.
	double data;
	double tempData; // Will replace after talking to team about how to best do data.
public:

	SpacePacket(int version, bool type, bool flag, int identifier, int groupFlag, int sourceSeq, double data);

	bool setSpacePacketHeader(int version, bool type, bool flag, int identifier, int groupFlag, int sourceSeq);

	long getSpacePacketHeader();

	int getSpacePacketLength(double data);//Data needs to be replaced

};

#endif SpacePacketProtocol_H