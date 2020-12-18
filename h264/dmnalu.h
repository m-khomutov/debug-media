#pragma once

#include <stdint.h>

namespace dm {
    namespace h264 {
        const char Unspec  =  0;
        const char NonIDR  =  1;
        const char SDP_A   =  2;
        const char SDP_B   =  3;
        const char SDP_C   =  4;
        const char IDR     =  5;
        const char SEI     =  6;
        const char SPS     =  7;
        const char PPS     =  8;
        const char DELIM   =  9;
        const char EOSeq   = 10;
        const char EOStream= 11;
        const char Filler  = 12;
        const char SPSext  = 13;
        const char Prefix  = 14;
        const char SubsetSPS = 15;
        const char DepthPS = 16;
        const char AuxSlice= 19;
        const char SliceExt= 20;
        const char Slice3D = 21;
        const char STAP_A = 24;
        const char STAP_B = 25;
        const char MTAP_A = 26;
        const char MTAP_B = 27;
        const char FU_A   = 28;
        const char FU_B   = 29;

        struct FUHeader {
            char start;
            char end;
            char type;
        };

        struct Header {
            uint8_t f;
            uint8_t nri;
            uint8_t type;

            FUHeader fuhdr;
            uint8_t futype;

            const uint8_t * payload;

            Header( const uint8_t * data );
        };
    }  // namespace h264
}  // namespace dm
