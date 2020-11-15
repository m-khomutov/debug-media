#include "dmnalu.h"

#include <ostream>

std::ostream& operator <<( std::ostream& out, const dm::h264::Header & hdr ) {
    out << "f: " << int(hdr.f) << " nri: " << int(hdr.nri) << " ";
    switch( hdr.type ) {
        case dm::h264::NonIDR:
            out << "NonIDR";
            break;
        case dm::h264::IDR:
            out << "IDR";
            break;
        case dm::h264::SEI:
            out << "SEI";
            break;
        case dm::h264::SPS:
            out << "SPS";
            break;
        case dm::h264::PPS:
            out << "PPS";
            break;
        case dm::h264::DELIM:
            out << "Delim";
            break;
        case dm::h264::UNIT:
            out << "Unit";
            break;
        case dm::h264::STAP_A:
            out << "STAP_A";
            break;
        case dm::h264::STAP_B:
            out << "STAP_B";
            break;
        case dm::h264::MTAP_A:
            out << "MTAP_A";
            break;
        case dm::h264::MTAP_B:
            out << "MTAP_B";
            break;
        case dm::h264::FU_A:
            out << "FU_A";
            break;
        case dm::h264::FU_B:
            out << "FU_B";
            break;
    }
    return out;
}

dm::h264::Header::Header( const uint8_t* data ) {
    off_t off = 0;
    f = (data[ off ] & 0x80) >> 7;
    nri = (data[ off ] & 0x60) >> 5;
    type = (data[ off ] & 0x1F);

    if ( type == FU_A || type == FU_B ) {
        fuhdr.start = ((data[off + 1])&0x80)>>7;
        fuhdr.end   = ((data[off + 1])&0x40)>>6;
        fuhdr.type  = ((data[off + 1])&0x1F);

        futype = ((f&0x01)<<7)|((nri&0x03)<<5)|(fuhdr.type&0x1F);
        off += 2;
    }
    payload = &data[ off ];
}
