//
// Created by mkh on 15.11.2020.
//
#pragma once

#include "dmsdp.h"

namespace dm {
    namespace rtsp {
        class MediaSession {
        public:
            void setTransport( const char* line );
            void setId( const char * line );

            MediaDescription * description() {
                return &m_media_description;
            }
            const std::string & id() const {
                return m_id;
            }

        private:
            MediaDescription m_media_description;
            std::string m_id;
            uint32_t m_timeout;
            uint8_t m_interleaved_port[2];
        };
    }  // namespace rtsp
}  // namespace dm
