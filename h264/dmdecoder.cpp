//
// Created by mkh on 16.11.2020.
//

#include "dmdecoder.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>

namespace {
    int setLock( void** m, AVLockOp op ) {
    std::mutex*& mutex = *(std::mutex**)m;
    switch( op ) {
        case AV_LOCK_CREATE:
            mutex = new std::mutex();
            break;
        case AV_LOCK_DESTROY:
            delete mutex;
            break;
        case AV_LOCK_OBTAIN:
            mutex->lock();
            break;
        case AV_LOCK_RELEASE:
            mutex->unlock();
            break;
    }
    return 0;
}
}  // namespace

dm::h264::Decoder::Decoder() {
    av_register_all();
    avcodec_register_all();
    av_lockmgr_register( setLock );

    if( !(m_codec = avcodec_find_decoder( AV_CODEC_ID_H264 )) )
        throw std::logic_error( "avcodec_find_decoder failed" );

    m_codecCtx.reset( avcodec_alloc_context3( m_codec ) );
    if( !m_codecCtx.get() )
        throw std::logic_error( "avcodec_alloc_context3 failed" );

    m_codecCtx->codec = m_codec;
    m_codecCtx->extradata = nullptr;
    m_codecCtx->extradata_size = 0;
    m_codecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    m_codecCtx->codec_id   = AV_CODEC_ID_H264;
    m_codecCtx->pix_fmt    = AV_PIX_FMT_YUV420P;
    m_codecCtx->codec_tag  = (('H')|('2' << 8)|('6' << 16)|('4' << 24));
    m_codecCtx->thread_count = 1;//std::thread::hardware_concurrency();

    m_avFrame.reset( av_frame_alloc() );
    if( !m_avFrame.get() )
        throw std::logic_error( "av_frame_alloc failed" );

    if( avcodec_open2( m_codecCtx.get(), m_codec, nullptr ) < 0 )
        throw std::logic_error( "avcodec_open2 failed" );
}

dm::h264::Decoder::~Decoder() {
    avcodec_close( m_codecCtx.get() );
}

AVFrame * dm::h264::Decoder::decode( std::vector< uint8_t > & data ) {
    AVPacket p;
    std::unique_ptr< AVPacket, std::function< void(AVPacket*) > >  packet( &p, [](AVPacket*p){ av_free_packet( p ); });

    av_init_packet( packet.get() );
    packet->size = data.size();
    packet->data = data.data();

    int got_picture = 0;
    int decode_times_with_zero_len = 0;
    while( packet->size >= 0 ) {
        int len = avcodec_decode_video2( m_codecCtx.get(), m_avFrame.get(), &got_picture, packet.get() );
        if( len < 0 ) {
            std::cerr << "Error decoding video frame (avcodec_decode_video2)\n";
            return nullptr;
        }
        if( got_picture )
            break;

        packet->size -= len;
        packet->data += len;
        if( len == 0 ) {
            ++decode_times_with_zero_len;
            if( decode_times_with_zero_len > 2 ) {
                std::cerr << "Error decoding frame (input buffer won't advance)\n";
                return nullptr;
            }
        }
    }

    if( got_picture ) {
        if( av_frame_get_decode_error_flags( m_avFrame.get() ) || (m_avFrame->flags & AV_FRAME_FLAG_CORRUPT) ) {
            std::cerr << "Error decoding frame (corrupt frame)\n";
            return nullptr;
        }
        // на кодирование m_avFrame->data, m_avFrame->linesize, m_avFrame->width, m_avFrame->height
        return m_avFrame.get();
    }
    else
        std::cerr << "Error decoding h264 frame (invalid data, no picture)\n";
    return nullptr;
}
