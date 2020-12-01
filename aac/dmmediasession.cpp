//
// Created by mkh on 25.11.2020.
//

#include "dmmediasession.h"
#include "dmauheadersection.h"

namespace {
   uint32_t SamplingFrequencyTable[] = { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350 };
   uint32_t frequencyIndex( int frequency ) {
       for (int i(0); i < sizeof(SamplingFrequencyTable) / sizeof(SamplingFrequencyTable[0]); i++) {
           if( SamplingFrequencyTable[i] == frequency )
               return i;
       }
       return 4;
   }
}  // namespace

std::ostream& operator <<( std::ostream& out, const dm::rtp::Header & h );




dm::aac::MediaSession::MediaSession( const rtsp::MediaDescription & description, rtsp::Connection * connection )
: rtsp::MediaSession( description, connection ),m_auheader_fields( description.fmtp ) {
    size_t p;
    if( (p = description.rtpmap.find( '/')) != std::string::npos ) {
        int frequency = strtol( description.rtpmap.data()+(p+1), nullptr, 10 );
        if( (p = description.rtpmap.find( '/', p+1)) != std::string::npos ) {
            uint8_t channels = strtol( description.rtpmap.data()+(p+1), nullptr, 10 );
            m_decoder.reset( new Decoder( frequency, channels ) );
            m_player.reset( new alsa::Player( frequency, channels, SND_PCM_FORMAT_FLOAT ) );
        }
    }
}

void dm::aac::MediaSession::receiveInterleaved( uint8_t *data, size_t datasz ) {
    rtsp::MediaSession::receiveInterleaved( data, datasz );
//    std::cerr << "rtp: " << m_rtp_header << std::endl;
    if( m_rtp_header.payload_type == m_payload_type ) {
        AuHeaderSection header_section( m_rtp_header.payload, m_auheader_fields );
        data = header_section.payload();
        for( auto h : header_section ) {
            if( m_decoder && m_player )
                m_player->onFrame( m_decoder->decode( data, h.size() ), m_decoder->frame() );
            data += h.size();
        }
    }
}