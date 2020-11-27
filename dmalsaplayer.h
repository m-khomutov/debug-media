//
// Created by mkh on 26.11.2020.
//
#pragma once

#include "core/dmbaseplayer.h"

#include <alsa/asoundlib.h>
#include <memory>
#include <functional>

namespace dm {
    namespace alsa {
        class Player : public BasePlayer {
        public:
            Player( uint16_t samplerate, uint8_t channels, snd_pcm_format_t fmt );

            void run() override {}
            void onFrame( int len, AVFrame * frame ) override;

        private:
            template< typename T >
            using SndPcmPtr = std::unique_ptr< T, std::function< void(T*) > >;
            SndPcmPtr< snd_pcm_t > m_pcm;
        };
    }  // namespace alsa
}  // namespace dm
