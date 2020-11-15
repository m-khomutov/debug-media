//
// Created by mkh on 15.11.2020.
//
#pragma once

#include "dmsdp.h"

namespace dm {
    namespace rtp {
        struct Header {
            uint8_t version;
            uint8_t padding;
            uint8_t extension;
            uint8_t csrc_count;
            uint8_t marker;
            uint8_t payload_type;
            uint16_t sequence_number;
            uint32_t timestamp;

            uint32_t ssrc;
            uint32_t csrc_list[ 0x0F ];

            const uint8_t * payload;

            Header( const uint8_t * data );
        };
    }  // namespace rtp

    namespace rtsp {
        class MediaSession {
        public:
            void setTransport( const char* line );
            void setId( const char * line );

            void receiveInterleaved( const uint8_t * data, size_t datasz );

            MediaDescription * description() {
                return &m_media_description;
            }
            const std::string & id() const {
                return m_id;
            }
            uint8_t channel() const {
                return m_interleaved_channel[0];
            }

        private:
            MediaDescription m_media_description;
            std::string m_id;
            uint32_t m_timeout;
            uint8_t m_interleaved_channel[2];
        };
    }  // namespace rtsp
}  // namespace dm
