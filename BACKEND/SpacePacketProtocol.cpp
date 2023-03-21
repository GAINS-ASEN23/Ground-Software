/* BACKEND SPP Packaging FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Brian Trybus
* Purpose: This is the file to Pack data in the SPP format.
*/



#include "SpacePacketProtocol.h"



// SpacePacket constructor
SpacePacket::SpacePacket(int version, bool type, bool flag, int identifier, int groupFlag, int sourceSeq, double data)
{
    /*
    Makes the basic space packet with 32 bit header in the following order:
        int version: Version no: 3 bits
        bool type: Type Indicator: 1 bit
        bool flag: PCKT- SEC HDR FLAG: 1 bit
        int identifier: Application Process Identfeir: 11 bits
        int groupFlag: Grouping flag(first, mid, last, or NA): 2 bits
        int sourceSeq: Source Sequence Count: 14 bits
        double Data: Temp will be changed but its just some data to pack.
    */


    //First makes primaery header 
    if (!setSpacePacketHeader(version, type, flag, identifier, groupFlag, sourceSeq))
    {
        // put an error flag here saying primeary header is not valid.
    }

    //Now packs the data.
    tempData = data;
}

// SpacePacket member functions

bool SpacePacket::setSpacePacketHeader(int version, bool type, bool flag, int identifier, int groupFlag, int sourceSeq)
{
    bool correct = 1;//If any data is not formatted right then this returns 0;

    int temp = 0;

    correct = (version < 8) * correct;
    correct = (identifier <= 0x7ff) * correct;
    correct = (groupFlag <= 3) * correct;
    correct = (sourceSeq <= 0x3fff) * correct;

    primaryHeader = (long)((version << 29) | (type << 28) | (flag << 27) | (identifier << 16) | (groupFlag << 14) | (sourceSeq));

    packetLength = getSpacePacketLength(data);
}

long SpacePacket::getSpacePacketHeader()
{

}

int SpacePacket::getSpacePacketLength(double data)
{

}