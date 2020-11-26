//
// Created by mkh on 16.11.2020.
//

#include "dmdecoder.h"
#include <iostream>
#include <thread>
#include <functional>

dm::h264::Decoder::Decoder() : av::Decoder( AV_CODEC_ID_H264 ) {
    m_codecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    m_codecCtx->codec_id   = AV_CODEC_ID_H264;
    m_codecCtx->pix_fmt    = AV_PIX_FMT_YUV420P;
    m_codecCtx->codec_tag  = (('H')|('2' << 8)|('6' << 16)|('4' << 24));
    m_codecCtx->thread_count = 1;//std::thread::hardware_concurrency();

    if( avcodec_open2( m_codecCtx.get(), m_codec, nullptr ) < 0 )
        throw std::logic_error( "avcodec_open2 failed" );
}

dm::h264::Decoder::~Decoder() {
    avcodec_close( m_codecCtx.get() );
}

int dm::h264::Decoder::decode( uint8_t * data, size_t size ) {
    AVPacket p;
    std::unique_ptr< AVPacket, std::function< void(AVPacket*) > >  packet( &p, [](AVPacket*p){ av_free_packet( p ); });

    av_init_packet( packet.get() );
    packet->size = size;
    packet->data = data;

    int got_picture = 0;
    int decode_times_with_zero_len = 0;
    int len;
    while( packet->size >= 0 ) {
        len = avcodec_decode_video2( m_codecCtx.get(), m_avFrame.get(), &got_picture, packet.get() );
        if( len < 0 ) {
            std::cerr << "Error decoding video frame (avcodec_decode_video2)\n";
            return -1;
        }
        if( got_picture )
            break;

        packet->size -= len;
        packet->data += len;
        if( len == 0 ) {
            ++decode_times_with_zero_len;
            if( decode_times_with_zero_len > 2 ) {
                std::cerr << "Error decoding frame (input buffer won't advance)\n";
                return -1;
            }
        }
    }

    if( got_picture ) {
        if( av_frame_get_decode_error_flags( m_avFrame.get() ) || (m_avFrame->flags & AV_FRAME_FLAG_CORRUPT) ) {
            std::cerr << "Error decoding frame (corrupt frame)\n";
            return -1;
        }
        // на кодирование m_avFrame->data, m_avFrame->linesize, m_avFrame->width, m_avFrame->height
        return len;
    }
    else
        std::cerr << "Error decoding h264 frame (invalid data, no picture)\n";
    return -1;
}
