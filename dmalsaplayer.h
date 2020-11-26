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
            using SndPcmPtr = std::unique_ptr< snd_pcm_t, std::function< void(snd_pcm_t*) > >;
            SndPcmPtr m_pcm;
        };
    }  // namespace alsa
}  // namespace dm
