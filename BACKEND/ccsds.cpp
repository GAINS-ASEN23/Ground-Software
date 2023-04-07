/* CCSDS Space Protocol Packet Functions File
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Brian Trybus,
* Purpose: This has functions help with using the CCSDS Space Packet Structs
*/

#include "ccsds.h";


headerData readHeader(CCSDS_PriHdr_t input);

CCSDS_PriHdr_t writeHeader(int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag);

GAINS_TLM_PACKET GAINS_TLM_PACKET_constructor(double position_x, double position_y, double position_z, double velocity_x, double velocity_y, double velocity_z, float time, int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag);

GAINS_STAR_PACKET GAINS_STAR_PACKET_constructor(double betaAngle1, double betaAngle2, double betaAngle3, double betaAngle4, float time, int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag);


CCSDS_PriHdr_t writeHeader(int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag) {

    CCSDS_PriHdr_t header;

    header.StreamId = apID | (secondHeader << 11) | (type << 12) | (version << 13);

    header.Sequence = seqCount | (segFlag << 14);

    header.Length = 0;

    return header;

}

GAINS_TLM_PACKET GAINS_TLM_PACKET_constructor(double position_x, double position_y, double position_z, double velocity_x, double velocity_y, double velocity_z, float time, int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag) {

    GAINS_TLM_PACKET tlmPacket;

    tlmPacket.position_x = position_x;
    tlmPacket.position_y = position_y;
    tlmPacket.position_z = position_z;
    tlmPacket.velocity_x = velocity_x;
    tlmPacket.velocity_y = velocity_y;
    tlmPacket.velocity_z = velocity_z;

    tlmPacket.FullHeader.SpacePacket.Hdr = writeHeader(apID, secondHeader, type, version, seqCount, segFlag);

    tlmPacket.FullHeader.SpacePacket.Hdr.Length = sizeof(tlmPacket);

    tlmPacket.FullHeader.Sec.Time = (double)time;

    return tlmPacket;
}

GAINS_STAR_PACKET GAINS_STAR_PACKET_constructor(double betaAngle1, double betaAngle2, double betaAngle3, double betaAngle4, float time, int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag) {

    GAINS_STAR_PACKET starPacket;

    starPacket.position_x = betaAngle1;
    starPacket.position_y = betaAngle1;
    starPacket.position_z = betaAngle1;
    starPacket.velocity_x = betaAngle1;

    starPacket.FullHeader.SpacePacket.Hdr = writeHeader(apID, secondHeader, type, version, seqCount, segFlag);

    starPacket.FullHeader.SpacePacket.Hdr.Length = sizeof(starPacket);

    starPacket.FullHeader.Sec.Time = (double)time;

    return starPacket;
}

headerData readHeader(CCSDS_PriHdr_t input) {

    headerData data;

    data.appId = 0x07FF & input.StreamId;
    data.secondHeader = (0x0800 & input.StreamId) >> 11;
    data.type = (0x1000 & input.StreamId) >> 12;
    data.version = (0xE000 & input.StreamId) >> 13;

    data.seqCount = 0x3FFF & input.Sequence;
    data.segFlag = (0xC000 & input.Sequence) >> 14;

    data.length = input.Length;

}