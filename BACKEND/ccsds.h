#pragma once
/* CCSDS Space Protocol Packet Header File
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Jason Popich, Brian Trybus, Cannon Palmer, Tucker Peyok, Ben McHugh
* Purpose: This is the header file which defines the CCSDS Space Packet Struct Format
*/

#ifndef _CCSDS_H_
#define _CCSDS_H_

#include <iostream>       // std::cout

#include "boost/array.hpp"

/*
** Type Definitions
*/

#define CCSDS_TIME_SIZE 8

/**********************************************************************
** Structure definitions for CCSDS headers.  All items in the structure
** must be aligned on 16-bit words.  Bitfields must be avoided since
** some compilers (such as gcc) force them into 32-bit alignment.
**
** CCSDS headers must always be in network byte order per the standard.
** MSB at the lowest address which is commonly refered to as "BIG Endian"
**
** CCSDS Space Packets can be version 1 or version 2.  Version 2 has
** an additional 32 bits for APID Qualifier fields in the secondary
** header. The primary header is unchanged.
**
**********************************************************************/

/*----- CCSDS packet primary header. -----*/

typedef struct {

    uint8_t   StreamId[2];  /* packet identifier word (stream ID) */
    /*  bits  shift   ------------ description ---------------- */
    /* 0x07FF    0  : application ID                            */
    /* 0x0800   11  : secondary header: 0 = absent, 1 = present */
    /* 0x1000   12  : packet type:      0 = TLM, 1 = CMD        */
    /* 0xE000   13  : CCSDS version:    0 = ver 1, 1 = ver 2    */

    uint8_t   Sequence[2];  /* packet sequence word */
    /*  bits  shift   ------------ description ---------------- */
    /* 0x3FFF    0  : sequence count                            */
    /* 0xC000   14  : segmentation flags:  3 = complete packet  */

    uint8_t  Length[2];     /* packet length word */
    /*  bits  shift   ------------ description ---------------- */
    /* 0xFFFF    0  : (total packet length) - 7                 */

} CCSDS_PriHdr_t;

/*----- CCSDS command secondary header. -----*/

typedef struct {

    uint16_t  Command;      /* command secondary header */
    /*  bits  shift   ------------ description ---------------- */
    /* 0x00FF    0  : checksum, calculated by ground system     */
    /* 0x7F00    8  : command function code                     */
    /* 0x8000   15  : reserved, set to 0                        */

} CCSDS_CmdSecHdr_t;

/*----- CCSDS telemetry secondary header. -----*/

typedef struct {

    float  Time;  /* packet time float */
    uint8_t Mode; /* packet mode word */

} CCSDS_TlmSecHdr_t; 

typedef struct
{
    /*
     * In Version 1 mode, the standard / non-APID qualified header is used for all packets
     */
    CCSDS_PriHdr_t      Hdr;    /**< Complete "version 1" (standard) header */

} CCSDS_SpacePacket_t;

/*----- Generic combined command header. -----*/

typedef struct
{
    CCSDS_SpacePacket_t  SpacePacket;   /**< \brief Standard Header on all packets  */
    CCSDS_CmdSecHdr_t    Sec;
} CCSDS_CommandPacket_t;

/*----- Generic combined telemetry header. -----*/

typedef struct
{
    CCSDS_SpacePacket_t  SpacePacket; /**< \brief Standard Header on all packets */
    CCSDS_TlmSecHdr_t    Sec;
} CCSDS_TelemetryPacket_t;

/*----- Combined telemetry packet -----*/
struct GAINS_TLM_PACKET { /* Length will be 72 bytes due to data alignment */
    CCSDS_TelemetryPacket_t  FullHeader;
    uint8_t     ci_command_error_count{ 0 };
    double      position_x{ 0 };
    double      position_y{ 0 };
    double      position_z{ 0 };
    double      velocity_x{ 0 };
    double      velocity_y{ 0 };
    double      velocity_z{ 0 };
};

/*----- Combined Star Tracker packet -----*/
struct GAINS_STAR_PACKET { /* Length will be 72 bytes due to data alignment */
    CCSDS_TelemetryPacket_t     FullHeader;
    uint8_t     ci_command_error_count{ 0 };
    double      betaAngle1{ 0 };
    double      betaAngle2{ 0 };
    double      betaAngle3{ 0 };
    double      betaAngle4{ 0 };
    double      filler1 = 0;
    double      filler2 = 0;

};

struct headerData {
    int appId = 0;
    bool secondHeader = 0;
    bool type = 0;
    int version = 0;
    int seqCount = 0;
    int segFlag = 0;
    int length = 0;
};


#define CFE_SB_TLM_HDR_SIZE sizeof(CCSDS_TelemetryPacket_t)

CCSDS_PriHdr_t writeHeader(int apID, bool secondHeader, bool type, int version, int seqCount, int segFlag);
headerData readHeader(CCSDS_PriHdr_t hdr);

GAINS_TLM_PACKET GAINS_TLM_PACKET_constructor(double position_x, double position_y, double position_z, double velocity_x, double velocity_y, double velocity_z, float time, int apID, bool secondHeader, bool type, int version, int seqCount, int segFlag);
GAINS_STAR_PACKET GAINS_STAR_PACKET_constructor(double betaAngle1, double betaAngle2, double betaAngle3, double betaAngle4, float time, int apID, bool secondHeader, bool type, int version, int seqCount, int segFlag);

GAINS_TLM_PACKET read_TLM_Packet(boost::array<uint8_t, 72> recv_buffer);
GAINS_STAR_PACKET read_STAR_Packet(boost::array<uint8_t, 72> recv_buffer);

void print_GAINS_TLM_PACKET(GAINS_TLM_PACKET tlm_packet);
void print_GAINS_STAR_PACKET(GAINS_STAR_PACKET tlm_packet);

#endif