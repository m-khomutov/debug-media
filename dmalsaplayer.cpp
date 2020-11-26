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

    m_pcm = SndPcmPtr( pcm, [](snd_pcm_t * pcm){ snd_pcm_close( pcm ); } );
    if( (rc = snd_pcm_set_params( m_pcm.get(), fmt, SND_PCM_ACCESS_RW_INTERLEAVED, channels, samplerate,
            1, samplerate * 8 * channels + 100 )) < 0 )
        throw std::logic_error( std::string( snd_strerror( rc ) ) );
}

void dm::alsa::Player::onFrame( int len , AVFrame *frame ) {
    if( len != -1 ) {
        snd_pcm_uframes_t count = 0;
        len = len / sizeof(float);
        do {
            snd_pcm_uframes_t frames = snd_pcm_writei( m_pcm.get(), (float*)frame->data[0] + count, len - count );
            if (frames == -EPIPE) {
                std::cerr << snd_strerror( frames ) << std::endl;
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