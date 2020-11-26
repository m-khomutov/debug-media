//
// Created by mkh on 16.11.2020.
//
#pragma once

#include "core/dmavdecoder.h"

namespace dm {
    namespace h264 {
    class Decoder : public av::Decoder {
        public:
            Decoder();
            ~Decoder();

            int decode( uint8_t * data, size_t size ) override;
        };
    }  // namespace h264
}  //namespace dm