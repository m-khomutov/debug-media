//
// Created by mkh on 16.11.2020.
//
#pragma once

#include <rtsp/dmmediasession.h>
#include <h264/dmdecoder.h>

namespace dm {
    namespace rtsp {
        class Connection;
    }
    namespace h264 {
        class MediaSession : public rtsp::MediaSession {
        public:
            MediaSession( const rtsp::MediaDescription & description, rtsp::Connection * connection );

            void receiveInterleaved( const uint8_t * data, size_t datasz ) override;

        private:
            Decoder m_decoder;

            std::vector< uint8_t > m_sps;
            std::vector< uint8_t > m_pps;
            std::vector< uint8_t > m_nalu;

        private:
            void f_set_sprop_parameter_sets( const std::string & sprop );
    };
    }  // namwspace h264
}  // namespace dm
