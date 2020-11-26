//
// Created by mkh on 25.11.2020.
//
#pragma once

#include "core/dmavdecoder.h"

namespace dm {
    namespace aac {
        class Decoder : public av::Decoder {
        public:
            Decoder( int sampleRate, uint8_t channels );
            ~Decoder();

            int decode( uint8_t * data, size_t size ) override;
        };

    }  // namespace aac
}  // namespace dm
