//
// Created by mkh on 15.11.2020.
//

#include "dmmediasession.h"

namespace dm {
    namespace rtp {
        std::ostream& operator <<( std::ostream& out, const dm::rtp::Header & h ) {
            out << "v=" << int(h.version)
                << " p=" << int(h.padding)
                << " x=" << int(h.extension)
                << " cc=" << int(h.csrc_count)
                << " m=" << int(h.marker)
                << " pt=" << int(h.payload_type)
                << " sn=" << h.sequence_number
                << " ts=" << h.timestamp
                << " ssrc=" << h.ssrc;
            if( h.csrc_count ) {
                out << " csrc={";
                for( uint8_t i(0); i < h.csrc_count; ++ i )
                    out << h.csrc_list[ i ];
                out << "}";
            }
            return out;
        }
    }  //namespace rtp
}  // namespace dm


dm::rtp::Header::Header( const uint8_t * data ) {
    off_t off = 0;
    version = ((data[ off ])>>6)&0x03;   // 2 bits
    padding = ((data[ off ])>>5)&0x01;   // 1 bit
    extension = ((data[ off ])>>4)&0x01; // 1 bit
    csrc_count = (data[ off ])&0x0F;     // 4 bits
    ++ off;

    marker = ((data[ off ])>>7)&0x01;  // 1 bit
    payload_type = (data[ off ])&0x7F; // 7 bits
    ++ off;

    sequence_number = ((data[ off ])<<8)|(data[off + 1]); // 16 bits
    off += sizeof(uint16_t);
    timestamp = ((data[ off ])<<24)|((data[off + 1])<<16)|((data[off + 2])<<8)|(data[off + 3]); // 32 bits
    off += sizeof( uint32_t );
    ssrc = ((data[ off ])<<24)|((data[off + 1])<<16)|((data[off + 2])<<8)|(data[off + 3]);  // 32 bits
    off += sizeof( uint32_t );

    for( uint8_t i(0); i < csrc_count; ++ i ) {
        csrc_list[ i ] = ((data[ off ])<<24)|((data[off + 1])<<16)|((data[off + 2])<<8)|(data[off + 3]); // 32 bits
        off += sizeof( uint32_t );
   }
   payload = data + off;
}

void dm::rtsp::MediaSession::receiveInterleaved( const uint8_t *data, size_t datasz ) {
    rtp::Header rtpHeader( data );
    std::cerr << "recvd  chan: " << int(m_interleaved_channel[0]) << " : sz=" << datasz << std::endl;
    std::cerr << rtpHeader << std::endl;
}

void dm::rtsp::MediaSession::setTransport( const char* line ) {
    const char* ptr = strstr( line, "interleaved=" );
    if( ptr ) {
        m_interleaved_channel[0] = ptr[12] - '0';
        m_interleaved_channel[1] = ptr[14] - '0';
    }
}

void dm::rtsp::MediaSession::setId( const char* line ) {
    const char* ptr = strchr( line, ';' );
    m_id = ptr ? std::string( line, ptr - line ) : std::string( line );

    if ((ptr = strstr(line, "timeout=")))
        m_timeout = strtol( ptr + 8, 0, 10 );
}
