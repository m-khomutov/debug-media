//
// Created by mkh on 16.11.2020.
//
#pragma once

extern "C" {
#include <libavutil/frame.h>
};

namespace dm {
    namespace rtsp {
        class MediaSession;
    }
    class BasePlayer {
    public:
        virtual ~BasePlayer() = default;

        virtual void run() = 0;
        virtual void onFrame( int len, AVFrame * frame ) = 0;
    };
}
