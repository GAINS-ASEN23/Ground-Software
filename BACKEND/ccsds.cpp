/* CCSDS Space Protocol Packet Functions File
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Brian Trybus, Cannon Palmer, Tucker Peyok, Ben McHugh
* Purpose: This has functions help with using the CCSDS Space Packet Structs
*/

#include "ccsds.h"

CCSDS_PriHdr_t writeHeader(int apID, bool secondHeader, bool type, int version, int seqCount, int segFlag) {

    CCSDS_PriHdr_t header;
    uint16_t StreamID = (0x07FF & apID) | ((0x1 & secondHeader) << 11) | ((0x1 & type) << 12) | ((0x7 & version) << 13);
    header.StreamId[0] = (0xFF00 & StreamID) >> 8;
    header.StreamId[1] = (0xFF & StreamID);

    uint16_t Sequence = (0x3FFF & seqCount) | ((0x3 & segFlag) << 14);
    header.Sequence[0] = (0xFF00 & Sequence) >> 8;
    header.Sequence[1] = (0xFF & Sequence);

    header.Length[0] = 0;
    header.Length[1] = 0;

    return header;

}

headerData readHeader(CCSDS_PriHdr_t hdr) {

    headerData header;

    header.appId = (int)(((0x07 & hdr.StreamId[0]) << 8) | hdr.StreamId[1]);
    header.secondHeader = (bool)((0x08 & hdr.StreamId[0])>>3);
    header.type = (bool)((0x10 & hdr.StreamId[0])>>4);
    header.version = (int)((0xE0 & hdr.StreamId[0])>>5);

    header.seqCount = (int)(((0x3F & hdr.Sequence[0]) << 8) | hdr.Sequence[1]);
    header.segFlag = (int)((0xC0 & hdr.Sequence[0])>>6);

    uint16_t Length = (int)((hdr.Length[0] << 8) | hdr.Length[1]);
    header.length = (int)(Length);

    std::cout << "Read the Packet Header \n";
    std::cout << "appID = " << header.appId << std::endl;
    std::cout << "secondHeader = " << header.secondHeader << std::endl;
    std::cout << "type = " << header.type << std::endl;
    std::cout << "version = " << header.version << std::endl;
    std::cout << "seqCount = " << header.seqCount << std::endl;
    std::cout << "segFlag = " << header.segFlag << std::endl;
    std::cout << "length = " << header.length << std::endl;

    return header;

}

GAINS_TLM_PACKET GAINS_TLM_PACKET_constructor(double position_x, double position_y, double position_z, double velocity_x, double velocity_y, double velocity_z, float time, int apID, bool secondHeader, bool type, int version, int seqCount, int segFlag) {

    GAINS_TLM_PACKET tlmPacket;

    tlmPacket.position_x = position_x;
    tlmPacket.position_y = position_y;
    tlmPacket.position_z = position_z;
    tlmPacket.velocity_x = velocity_x;
    tlmPacket.velocity_y = velocity_y;
    tlmPacket.velocity_z = velocity_z;

    tlmPacket.FullHeader.SpacePacket.Hdr = writeHeader(apID, secondHeader, type, version, seqCount, segFlag);

    uint16_t Length = sizeof(tlmPacket);
    tlmPacket.FullHeader.SpacePacket.Hdr.Length[0] = (0xFF00 & Length) >> 8;
    tlmPacket.FullHeader.SpacePacket.Hdr.Length[1] = (0xFF & Length);

    tlmPacket.FullHeader.Sec.Time = time;
    tlmPacket.FullHeader.Sec.Mode = 0; // Telemetry Packets are defined as using mode 0

    return tlmPacket;
}

GAINS_STAR_PACKET GAINS_STAR_PACKET_constructor(double betaAngle1, double betaAngle2, double betaAngle3, double betaAngle4, float time, int apID, bool secondHeader, bool type, int version, int seqCount, int segFlag) {

    GAINS_STAR_PACKET starPacket;

    starPacket.betaAngle1 = betaAngle1;
    starPacket.betaAngle2 = betaAngle2;
    starPacket.betaAngle3 = betaAngle3;
    starPacket.betaAngle4 = betaAngle4;

    starPacket.FullHeader.SpacePacket.Hdr = writeHeader(apID, secondHeader, type, version, seqCount, segFlag);

    size_t Length = sizeof(starPacket);
    starPacket.FullHeader.SpacePacket.Hdr.Length[0] = (0xFF00 & Length) >> 8;
    starPacket.FullHeader.SpacePacket.Hdr.Length[1] = 0xFF & Length;

    starPacket.FullHeader.Sec.Time = time;
    starPacket.FullHeader.Sec.Mode = 1; // Star Packets are defined as using mode 1

    return starPacket;
}

GAINS_TLM_PACKET read_TLM_Packet(boost::array<uint8_t, 72> recv_buffer) { 
    // we need to program this to decode a buffer (an array of uint8_t) and put it into the SPP format
    // incoming buffer will be 72 bytes long (72 uint8_t). Data from buffer comes in Little Endian
    GAINS_TLM_PACKET data;

    int offset = 0;

    data.FullHeader.SpacePacket.Hdr.StreamId[0] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.StreamId[1] = recv_buffer[offset + 1];
    offset = offset + 2;

    data.FullHeader.SpacePacket.Hdr.Sequence[0] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.Sequence[1] = recv_buffer[offset + 1];
    offset = offset + 2;

    data.FullHeader.SpacePacket.Hdr.Length[0] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.Length[1] = recv_buffer[offset + 1];
    offset = offset + 2 + 2;

    memcpy(&data.FullHeader.Sec.Time, &recv_buffer[offset], sizeof(data.FullHeader.Sec.Time));
    offset = offset + 4;

    data.FullHeader.Sec.Mode = recv_buffer[offset];
    offset = offset + 1 + 3;

    data.ci_command_error_count = recv_buffer[offset];
    offset = offset + 1 + 7;

    memcpy(&data.position_x, &recv_buffer[offset], sizeof(data.position_x));
    offset = offset + 8;

    memcpy(&data.position_y, &recv_buffer[offset], sizeof(data.position_y));
    offset = offset + 8;

    memcpy(&data.position_z, &recv_buffer[offset], sizeof(data.position_z));
    offset = offset + 8;

    memcpy(&data.velocity_x, &recv_buffer[offset], sizeof(data.velocity_x));
    offset = offset + 8;

    memcpy(&data.velocity_y, &recv_buffer[offset], sizeof(data.velocity_y));
    offset = offset + 8;

    memcpy(&data.velocity_z, &recv_buffer[offset], sizeof(data.velocity_z));
    offset = offset + 8;

    return data;
}

GAINS_STAR_PACKET read_STAR_Packet(boost::array<uint8_t, 72> recv_buffer) { 
    // we need to program this to decode a buffer (an array of uint8_t) and put it into the SPP format
    // incoming buffer will be 72 bytes long (72 uint8_t). Data from buffer comes in Little Endian
    GAINS_STAR_PACKET data;

    int offset = 0;

    data.FullHeader.SpacePacket.Hdr.StreamId[0] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.StreamId[1] = recv_buffer[offset + 1];
    offset = offset + 2;

    data.FullHeader.SpacePacket.Hdr.Sequence[0] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.Sequence[1] = recv_buffer[offset + 1];
    offset = offset + 2;

    data.FullHeader.SpacePacket.Hdr.Length[0] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.Length[1] = recv_buffer[offset + 1];
    offset = offset + 2 + 2;

    memcpy(&data.FullHeader.Sec.Time, &recv_buffer[offset], sizeof(data.FullHeader.Sec.Time));
    offset = offset + 4;

    data.FullHeader.Sec.Mode = recv_buffer[offset];
    offset = offset + 1 + 3;

    data.ci_command_error_count = recv_buffer[offset];
    offset = offset + 1 + 7;

    memcpy(&data.betaAngle1, &recv_buffer[offset], sizeof(data.betaAngle1));
    offset = offset + 8;

    memcpy(&data.betaAngle2, &recv_buffer[offset], sizeof(data.betaAngle2));
    offset = offset + 8;

    memcpy(&data.betaAngle3, &recv_buffer[offset], sizeof(data.betaAngle3));
    offset = offset + 8;

    memcpy(&data.betaAngle4, &recv_buffer[offset], sizeof(data.betaAngle4));
    offset = offset + 8;

    memcpy(&data.filler1, &recv_buffer[offset], sizeof(data.filler1));
    offset = offset + 8;

    memcpy(&data.filler2, &recv_buffer[offset], sizeof(data.filler2));
    offset = offset + 8;

    return data;
}

void print_GAINS_TLM_PACKET(GAINS_TLM_PACKET tlm_packet) {
    std::cout << " --- Gains TLM Packet Contents --- " << std::endl;

    std::cout << "StreamID[0] = " << unsigned(tlm_packet.FullHeader.SpacePacket.Hdr.StreamId[0]) << std::endl;
    std::cout << "StreamID[1] = " << unsigned(tlm_packet.FullHeader.SpacePacket.Hdr.StreamId[1]) << std::endl;

    std::cout << "Sequence[0] = " << unsigned(tlm_packet.FullHeader.SpacePacket.Hdr.Sequence[0]) << std::endl;
    std::cout << "Sequence[1] = " << unsigned(tlm_packet.FullHeader.SpacePacket.Hdr.Sequence[1]) << std::endl;

    std::cout << "Length[0] = " << unsigned(tlm_packet.FullHeader.SpacePacket.Hdr.Length[0]) << std::endl;
    std::cout << "Length[1] = " << unsigned(tlm_packet.FullHeader.SpacePacket.Hdr.Length[1]) << std::endl;

    std::cout << "Time = " << tlm_packet.FullHeader.Sec.Time << std::endl;

    std::cout << "Mode = " << unsigned(tlm_packet.FullHeader.Sec.Mode) << std::endl;

    std::cout << "Error Count = " << unsigned(tlm_packet.ci_command_error_count) << std::endl;

    std::cout << "Position X = " << tlm_packet.position_x << std::endl;
    std::cout << "Position Y = " << tlm_packet.position_y << std::endl;
    std::cout << "Position Z = " << tlm_packet.position_z << std::endl;

    std::cout << "Velocity X = " << tlm_packet.velocity_x << std::endl;
    std::cout << "Velocity Y = " << tlm_packet.velocity_y << std::endl;
    std::cout << "Velocity Z = " << tlm_packet.velocity_z << std::endl;
}

void print_GAINS_STAR_PACKET(GAINS_STAR_PACKET star_packet) {
    std::cout << " --- Gains STAR Packet Contents --- " << std::endl;

    std::cout << "StreamID[0] = " << unsigned(star_packet.FullHeader.SpacePacket.Hdr.StreamId[0]) << std::endl;
    std::cout << "StreamID[1] = " << unsigned(star_packet.FullHeader.SpacePacket.Hdr.StreamId[1]) << std::endl;

    std::cout << "Sequence[0] = " << unsigned(star_packet.FullHeader.SpacePacket.Hdr.Sequence[0]) << std::endl;
    std::cout << "Sequence[1] = " << unsigned(star_packet.FullHeader.SpacePacket.Hdr.Sequence[1]) << std::endl;

    std::cout << "Length[0] = " << unsigned(star_packet.FullHeader.SpacePacket.Hdr.Length[0]) << std::endl;
    std::cout << "Length[1] = " << unsigned(star_packet.FullHeader.SpacePacket.Hdr.Length[1]) << std::endl;

    std::cout << "Time = " << star_packet.FullHeader.Sec.Time << std::endl;

    std::cout << "Mode = " << unsigned(star_packet.FullHeader.Sec.Mode) << std::endl;

    std::cout << "Error Count = " << unsigned(star_packet.ci_command_error_count) << std::endl;

    std::cout << "Beta Angle 1 = " << star_packet.betaAngle1 << std::endl;
    std::cout << "Beta Angle 3 = " << star_packet.betaAngle2 << std::endl;
    std::cout << "Beta Angle 3 = " << star_packet.betaAngle3 << std::endl;
    std::cout << "Beta Angle 4 = " << star_packet.betaAngle4 << std::endl;

    //std::cout << "Filler 1 = " << star_packet.filler1 << std::endl;
    //std::cout << "Filler 2 = " << star_packet.filler2 << std::endl;
}