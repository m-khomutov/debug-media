//
// Created by mkh on 25.11.2020.
//
#pragma once

#include <rtsp/dmmediasession.h>
#include "dmdecoder.h"
#include "dmauheadersection.h"
#include "dmalsaplayer.h"

#include <memory>

namespace dm {
    namespace rtsp {
        class Connection;
    }
    namespace aac {
        class MediaSession : public rtsp::MediaSession {
        public:
            MediaSession( const rtsp::MediaDescription & description, rtsp::Connection * connection );

            void receiveInterleaved( uint8_t * data, size_t datasz ) override;

        private:
            std::unique_ptr< Decoder > m_decoder;
            std::unique_ptr< BasePlayer > m_player;
            AuHeader::Fields m_auheader_fields;
        };

    }  // namespace aac
}  // namespace dm