//
// Created by mkh on 16.11.2020.
//
#pragma once

#include <rtsp/dmmediasession.h>
#include <h264/dmdecoder.h>
#include "h264/dmsps.h"
#include "h264/parameters/dmpps.h"

#include <memory>

namespace dm {
    namespace rtsp {
        class Connection;
    }
    namespace h264 {
        class MediaSession : public rtsp::MediaSession {
        public:
            MediaSession( const rtsp::MediaDescription & description, rtsp::Connection * connection );

            void receiveInterleaved( uint8_t * data, size_t datasz ) override;

        private:
            Decoder m_decoder;

            std::unique_ptr< Sps > m_sps;
            std::unique_ptr< Pps > m_pps;

            std::vector< uint8_t > m_sps_unit;
            std::vector< uint8_t > m_pps_unit;
            std::vector< uint8_t > m_vcl_unit;

        private:
            void f_set_sprop_parameter_sets( const std::string & sprop );
    };
    }  // namwspace h264
}  // namespace dm
