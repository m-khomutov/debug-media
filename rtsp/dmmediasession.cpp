//
// Created by mkh on 15.11.2020.
//

#include "dmmediasession.h"
#include "h264/dmmediasession.h"

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


dm::rtsp::MediaSession * dm::rtsp::MediaSession::create( const MediaDescription &description, Connection * connection ) {
    if( description.rtpmap.find( "H264" ) != std::string::npos ) {
        return new h264::MediaSession( description, connection );
    }
    return new MediaSession( description, connection );
}

dm::rtsp::MediaSession::MediaSession( const MediaDescription & description, Connection * connection )
: m_media_description( description ),m_connection( connection ) {
    size_t pos = m_media_description.rtpmap.find( ':' );
    m_payload_type = strtol( description.rtpmap.substr( pos+1 ).c_str(), nullptr, 10 );

    m_range.first = 0ul;
    m_range.second = 0ul;
}

void dm::rtsp::MediaSession::receiveInterleaved( const uint8_t *data, size_t datasz ) {
    m_rtp_header = rtp::Header( data );
    if( m_rtp_header.payload_type == m_payload_type ) {
        if( !m_range.first )
            m_range.first = m_rtp_header.timestamp;
        m_range.second = m_rtp_header.timestamp;
        m_position = (m_range.second - m_range.first) / 90000;
    }
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