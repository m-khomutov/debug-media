//
// Created by mkh on 16.11.2020.
//

#include "dmmediasession.h"
#include "dmnalu.h"
#include "core/dmbase64.h"
#include "h264/dmsps.h"

std::ostream& operator <<( std::ostream& out, const dm::rtp::Header & h );
std::ostream& operator <<( std::ostream& out, const dm::h264::Header & h );

namespace {
    const uint8_t annexbdiv[4] = {0,0,0,1};
    void storeNalunit( const uint8_t * data, size_t datasz, std::vector< uint8_t > * nalu ) {
        std::copy( annexbdiv, annexbdiv+sizeof(annexbdiv), std::back_inserter( *nalu ) );
        std::copy( data, data+datasz, std::back_inserter( *nalu ) );
    }
}

dm::h264::MediaSession::MediaSession( const rtsp::MediaDescription & description, rtsp::Connection * connection )
: rtsp::MediaSession( description, connection ) {
    size_t pos = m_media_description.fmtp.find(  "sprop-parameter-sets=" );
    if( pos != std::string::npos ) {
        size_t p2 = m_media_description.fmtp.find( ';', pos );
        f_set_sprop_parameter_sets( m_media_description.fmtp.substr( pos+21, p2-pos-21) );
    }
    Sps sps( m_sps.data()+sizeof(annexbdiv), m_sps.size()-sizeof(annexbdiv) );
    m_player = BasePlayer::create( sps.width(), sps.height(), this );
    m_player->run();
}

void dm::h264::MediaSession::receiveInterleaved( const uint8_t *data, size_t datasz ) {
    rtsp::MediaSession::receiveInterleaved( data, datasz );
    h264::Header h264_hdr(m_rtp_header.payload );
    //std::cerr << "rtp: " << m_rtp_header << std::endl;
    //std::cerr << "nalu: " << h264_hdr << " : sz=" << datasz - (h264_hdr.payload-data) << std::endl;

    size_t len = datasz - (h264_hdr.payload - data);
    switch( h264_hdr.type ) {
        case h264::IDR:
            if( !m_sps.empty() )
                std::copy( m_sps.begin(), m_sps.end(), std::back_inserter( m_nalu ) );
            if( !m_pps.empty() )
                std::copy( m_pps.begin(), m_pps.end(), std::back_inserter( m_nalu ) );
        case h264::NonIDR:
            storeNalunit( h264_hdr.payload, len, &m_nalu );
            break;
        case h264::SPS:
            storeNalunit( h264_hdr.payload, len, &m_sps );
            return;
        case h264::PPS:
            storeNalunit( h264_hdr.payload, len, &m_pps );
            return;
        case h264::FU_A:
        case h264::FU_B:
            if( h264_hdr.fuhdr.start ) {
                if( h264_hdr.fuhdr.type == h264::IDR ) {
                    if( !m_sps.empty() )
                        std::copy( m_sps.begin(), m_sps.end(), std::back_inserter( m_nalu ) );
                    if( !m_pps.empty() )
                        std::copy( m_pps.begin(), m_pps.end(), std::back_inserter( m_nalu ) );
                }
                storeNalunit( &h264_hdr.futype, 1, &m_nalu );
            }
            std::copy( h264_hdr.payload, h264_hdr.payload+len, std::back_inserter( m_nalu ) );
            if( !h264_hdr.fuhdr.end )
                return;
            break;
        default:
            return;
    }
    m_player->onFrame( m_decoder.decode( m_nalu ) );
    m_nalu.clear();
}

void dm::h264::MediaSession::f_set_sprop_parameter_sets( const std::string & sprop ) {
    size_t p = sprop.find( ',' );
    if( p != std::string::npos ) {
        m_sps.assign( annexbdiv, annexbdiv+sizeof(annexbdiv) );
        Base64( sprop.substr( 0, p ) ).append( m_sps );
        m_pps.assign( annexbdiv, annexbdiv+sizeof(annexbdiv) );
        Base64( sprop.substr( p+1 ) ).append( m_pps );
    }
}
