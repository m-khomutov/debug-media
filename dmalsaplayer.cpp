//
// Created by mkh on 26.11.2020.
//

#include "dmalsaplayer.h"
#include <stdexcept>
#include <iostream>

namespace {
    const char * kSoundCardPortName = "default";
}

dm::alsa::Player::Player( uint16_t samplerate, uint8_t channels, snd_pcm_format_t fmt ) {
    int rc;
    snd_pcm_t * pcm;
    if( (rc = snd_pcm_open( &pcm, kSoundCardPortName, SND_PCM_STREAM_PLAYBACK, 0 )) < 0 )
        throw std::logic_error( std::string( snd_strerror( rc ) ) );

    m_pcm = SndPcmPtr< snd_pcm_t >( pcm, [](snd_pcm_t * pcm){ snd_pcm_close( pcm ); } );
    {
        SndPcmPtr< snd_pcm_hw_params_t > params;
        snd_pcm_hw_params_t * p;
        snd_pcm_hw_params_malloc( &p );
        params = SndPcmPtr< snd_pcm_hw_params_t >( p, [](snd_pcm_hw_params_t *p){ snd_pcm_hw_params_free( p ); });

        if( (rc = snd_pcm_hw_params_any( m_pcm.get(), params.get() )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        snd_pcm_hw_params_set_access( m_pcm.get(), params.get(), SND_PCM_ACCESS_RW_NONINTERLEAVED );
        snd_pcm_hw_params_set_format( m_pcm.get(), params.get(), SND_PCM_FORMAT_FLOAT );
        unsigned int exact_rate = samplerate;
        if( (rc = snd_pcm_hw_params_set_rate_near( m_pcm.get(), params.get(), &exact_rate, 0 )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        if( (rc = snd_pcm_hw_params_set_channels( m_pcm.get(), params.get(), channels )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        snd_pcm_uframes_t periods = 4;
        if( (rc = snd_pcm_hw_params_set_periods( m_pcm.get(), params.get(), periods, 0 )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        snd_pcm_uframes_t periodsize = 4096;
        snd_pcm_uframes_t size = (periodsize * periods) >> 2;
        if( (rc = snd_pcm_hw_params_set_buffer_size_near( m_pcm.get(), params.get(), &size )) < 0)
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        if( (rc = snd_pcm_hw_params( m_pcm.get(), params.get() )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
    }
    {
        SndPcmPtr< snd_pcm_sw_params_t > sw_params;
        snd_pcm_sw_params_t * swp;
        snd_pcm_sw_params_malloc( &swp );
        sw_params = SndPcmPtr< snd_pcm_sw_params_t >( swp, [](snd_pcm_sw_params_t *p){ snd_pcm_sw_params_free( p ); });

        if( (rc = snd_pcm_sw_params_current( m_pcm.get(), sw_params.get() )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        if( (rc = snd_pcm_sw_params_set_avail_min( m_pcm.get(), sw_params.get(), 1024 )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        if( (rc = snd_pcm_sw_params_set_start_threshold( m_pcm.get(), sw_params.get(), 1 )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
        if( (rc = snd_pcm_sw_params( m_pcm.get(), sw_params.get() )) < 0 )
            throw std::logic_error( std::string( snd_strerror( rc ) ) );
    }
}

void dm::alsa::Player::onFrame( int len , AVFrame *frame ) {
    if( len != -1 ) {
        snd_pcm_uframes_t count = 0;
        len = len / sizeof(float);
        do {
            snd_pcm_uframes_t frames = snd_pcm_writei( m_pcm.get(), (float*)frame->data[0] + count, len - count );
            if (frames == -EPIPE) {
                snd_pcm_prepare( m_pcm.get() );
                continue;
            }
            else if( frames < 0 ) {
                std::cerr << snd_strerror( frames ) << std::endl;
                return;
            }
            count += frames;
        }
        while( count < len );
    }
}