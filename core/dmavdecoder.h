//
// Created by mkh on 25.11.2020.
//
#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include "libavcodec/avcodec.h"
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <memory>
#include <vector>

namespace dm {
    namespace av {
        struct CodecSmartPtrDeleter {
            void operator()(void * ptr) { av_free( ptr ); }
        };
        template< typename T >
        using CodecSmartPtr=std::unique_ptr< T, CodecSmartPtrDeleter >;

        class Decoder {
        public:
            Decoder( AVCodecID codecId );
            virtual ~Decoder() = default;

            AVFrame * frame() {
                return m_avFrame.get();
            }

            virtual int decode( uint8_t * data, size_t size ) = 0;

        protected:
            AVCodec* m_codec;
            av::CodecSmartPtr< AVCodecContext > m_codecCtx;
            av::CodecSmartPtr< AVFrame > m_avFrame;
        };

    }  // namespace av
}  // namespace dm
