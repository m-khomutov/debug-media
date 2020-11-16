//
// Created by mkh on 16.11.2020.
//
#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include "libavcodec/avcodec.h"
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <vector>
#include <memory>

namespace dm {
    namespace h264 {
        struct AVCodecSmartPtrDeleter {
            void operator()(void * ptr) { av_free( ptr ); }
        };
        template< typename T >
        using AVCodecSmartPtr=std::unique_ptr< T, AVCodecSmartPtrDeleter >;
        class Decoder {
        public:
            Decoder();
            ~Decoder();

            AVFrame * decode( std::vector< uint8_t > & data );

        private:
            AVCodec* m_codec;
            AVCodecSmartPtr< AVCodecContext > m_codecCtx;
            AVCodecSmartPtr< AVFrame > m_avFrame;
        };
    }  // namespace h264
}  //namespace dm