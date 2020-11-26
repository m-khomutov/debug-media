//
// Created by mkh on 25.11.2020.
//

#include "dmdecoder.h"

#include <functional>
#include <iostream>

dm::aac::Decoder::Decoder( int sampleRate, uint8_t channels ) : av::Decoder( AV_CODEC_ID_AAC ) {
    m_codecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    m_codecCtx->codec_id   = AV_CODEC_ID_AAC;
    m_codecCtx->codec_tag  = (('A')|('A' << 8)|('C' << 16)|(' ' << 24));
    m_codecCtx->sample_fmt = AV_SAMPLE_FMT_NONE;
    m_codecCtx->sample_rate = sampleRate;
    m_codecCtx->channels = channels;
    m_codecCtx->channel_layout = av_get_default_channel_layout( m_codecCtx->channels );

    if( avcodec_open2( m_codecCtx.get(), m_codec, nullptr ) < 0 )
        throw std::logic_error( "avcodec_open2 failed" );
}

dm::aac::Decoder::~Decoder() {
    avcodec_close( m_codecCtx.get() );
}

int dm::aac::Decoder::decode( uint8_t * data, size_t size ) {
    AVPacket p;
    std::unique_ptr< AVPacket, std::function< void(AVPacket*) > >  packet( &p, [](AVPacket*p){ av_free_packet( p ); });

    av_init_packet( packet.get() );
    packet->size = size;
    packet->data = data;

    int got_frame = 0;
    while( packet->size > 0 ) {
        int len = avcodec_decode_audio4( m_codecCtx.get(), m_avFrame.get(), &got_frame, packet.get() );
        if( len < 0 ) {
            std::cerr <<"Could not decode audio frame. Negative return value\n";
            break;
        }
        if( got_frame ) {
            int ret = av_samples_get_buffer_size(nullptr, m_codecCtx->channels, m_avFrame->nb_samples, m_codecCtx->sample_fmt, 1);
            if( ret > 0 )
                return ret;
        }
        else
            std::cerr << "Error, got_frame == 0\n";

        packet->size -= len;
        packet->data += len;
    }
    return -1;
}