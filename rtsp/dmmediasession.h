//
// Created by mkh on 15.11.2020.
//
#pragma once

#include "core/dmbaseplayer.h"
#include "dmsdp.h"

#include <atomic>

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

            Header() = default;
            Header( const uint8_t * data );
        };
    }  // namespace rtp

    namespace rtsp {
        class Connection;

        using Range = std::pair< uint64_t, uint64_t >;

        class MediaSession {
        public:
            static MediaSession * create( const MediaDescription & description, Connection * connection );

            MediaSession( const MediaDescription & description, Connection * connection );
            virtual ~MediaSession() = default;

            void setTransport( const char* line );
            void setId( const char * line );

            virtual void receiveInterleaved( const uint8_t * data, size_t datasz );

            MediaDescription * description() {
                return &m_media_description;
            }
            Connection * connection() {
                return m_connection;
            }
            const std::string & id() const {
                return m_id;
            }
            uint8_t channel() const {
                return m_interleaved_channel[0];
            }
            double position() const {
                return m_position;
            }

        protected:
            MediaDescription m_media_description;
            std::string m_id;
            uint32_t m_timeout;
            uint32_t m_payload_type{0};
            uint8_t m_interleaved_channel[2];
            rtp::Header m_rtp_header;
            Range m_range;
            std::atomic< double > m_position;
            Connection * m_connection;
            BasePlayer * m_player {nullptr};
        };
    }  // namespace rtsp
}  // namespace dm
