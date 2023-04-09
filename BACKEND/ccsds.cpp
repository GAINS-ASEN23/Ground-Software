/* CCSDS Space Protocol Packet Functions File
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Brian Trybus,
* Purpose: This has functions help with using the CCSDS Space Packet Structs
*/

#include "ccsds.h"

CCSDS_PriHdr_t writeHeader(int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag) {

    CCSDS_PriHdr_t header;
    //apID = 0;
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

GAINS_TLM_PACKET GAINS_TLM_PACKET_constructor(double position_x, double position_y, double position_z, double velocity_x, double velocity_y, double velocity_z, float time, int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag) {

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
    tlmPacket.FullHeader.Sec.Mode = 7;

    return tlmPacket;
}

GAINS_STAR_PACKET GAINS_STAR_PACKET_constructor(double betaAngle1, double betaAngle2, double betaAngle3, double betaAngle4, float time, int apID, bool secondHeader, bool type, bool version, int seqCount, int segFlag) {

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
    starPacket.FullHeader.Sec.Mode = 1;

    return starPacket;
}


GAINS_TLM_PACKET readPacket(boost::array<uint8_t, 72> recv_buffer) { // we need to program this to decode a buffer (an array of uint8_t) and put it into the SPP format
   // incoming buffer will be 72 bytes long (72 uint8_t). Data from buffer comes in Little Endian
    GAINS_TLM_PACKET data;

    /*uint8_t temp_uint8_t = (uint8_t)recv_buffer[55];
    memcpy(&data.FullHeader.Sec.Mode, &temp_uint8_t, sizeof(float));*/

    data.FullHeader.Sec.Mode = recv_buffer[12];
    std::cout << "Mode value read in from recv_buffer[12] = " << unsigned(recv_buffer[12]) << "\n";

    //std::cout << "Received data packet mode = " << data.FullHeader.Sec.Mode << "\n";

    int offset = 0;

    data.FullHeader.SpacePacket.Hdr.StreamId[1] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.StreamId[0] = recv_buffer[offset + 1];
    offset = offset + 2;

    data.FullHeader.SpacePacket.Hdr.Sequence[1] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.Sequence[0] = recv_buffer[offset + 1];
    offset = offset + 2;

    data.FullHeader.SpacePacket.Hdr.Length[1] = recv_buffer[offset];
    data.FullHeader.SpacePacket.Hdr.Length[0] = recv_buffer[offset + 1];
    offset = offset + 2 + 2;

    data.FullHeader.Sec.Time = recv_buffer[3 + offset] << 24 |
        recv_buffer[2 + offset] << 16 |
        recv_buffer[1 + offset] << 8 |
        recv_buffer[0 + offset];
    offset = offset + 4;

    data.FullHeader.Sec.Mode = recv_buffer[offset];
    offset = offset + 1 + 3;

    data.ci_command_error_count = recv_buffer[offset];
    offset = offset + 1 + 7;

    data.position_x = recv_buffer[7 + offset] << 56 | // Read in the X position
        recv_buffer[6 + offset] << 48 |
        recv_buffer[5 + offset] << 40 |
        recv_buffer[4 + offset] << 32 |
        recv_buffer[3 + offset] << 24 |
        recv_buffer[2 + offset] << 16 |
        recv_buffer[1 + offset] << 8 |
        recv_buffer[0 + offset];
    offset = offset + 8;

    data.position_y = recv_buffer[7 + offset] << 56 | // Read in the Y position
        recv_buffer[6 + offset] << 48 |
        recv_buffer[5 + offset] << 40 |
        recv_buffer[4 + offset] << 32 |
        recv_buffer[3 + offset] << 24 |
        recv_buffer[2 + offset] << 16 |
        recv_buffer[1 + offset] << 8 |
        recv_buffer[0 + offset];
    offset = offset + 8;

    data.position_z = recv_buffer[7 + offset] << 56 | // Read in the Z position
        recv_buffer[6 + offset] << 48 |
        recv_buffer[5 + offset] << 40 |
        recv_buffer[4 + offset] << 32 |
        recv_buffer[3 + offset] << 24 |
        recv_buffer[2 + offset] << 16 |
        recv_buffer[1 + offset] << 8 |
        recv_buffer[0 + offset];
    offset = offset + 8;

    data.velocity_x = recv_buffer[7 + offset] << 56 | // Read in the X velocity
        recv_buffer[6 + offset] << 48 |
        recv_buffer[5 + offset] << 40 |
        recv_buffer[4 + offset] << 32 |
        recv_buffer[3 + offset] << 24 |
        recv_buffer[2 + offset] << 16 |
        recv_buffer[1 + offset] << 8 |
        recv_buffer[0 + offset];
    offset = offset + 8;

    data.velocity_y = recv_buffer[7 + offset] << 56 | // Read in the Y velocity
        recv_buffer[6 + offset] << 48 |
        recv_buffer[5 + offset] << 40 |
        recv_buffer[4 + offset] << 32 |
        recv_buffer[3 + offset] << 24 |
        recv_buffer[2 + offset] << 16 |
        recv_buffer[1 + offset] << 8 |
        recv_buffer[0 + offset];
    offset = offset + 8;

    data.velocity_z = recv_buffer[7 + offset] << 56 | // Read in the Z velocity
        recv_buffer[6 + offset] << 48 |
        recv_buffer[5 + offset] << 40 |
        recv_buffer[4 + offset] << 32 |
        recv_buffer[3 + offset] << 24 |
        recv_buffer[2 + offset] << 16 |
        recv_buffer[1 + offset] << 8 |
        recv_buffer[0 + offset];
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