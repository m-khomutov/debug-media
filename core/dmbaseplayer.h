//
// Created by mkh on 16.11.2020.
//
#pragma once

extern "C" {
#include <libavutil/frame.h>
};

namespace dm {
    class BasePlayer {
    public:
        static BasePlayer * create( int width, int height );

        virtual ~BasePlayer() = default;

        virtual void run() = 0;
        virtual void onFrame( AVFrame * frame ) =0;
    };
}