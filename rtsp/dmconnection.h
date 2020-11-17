#pragma once
#include "core/dmbasesession.h"
#include "core/dmbasereceiver.h"
#include "dmsdp.h"
#include "dmmediasession.h"

#include <memory>
#include <mutex>
#include <fstream>
#include <set>

namespace dm {
    namespace rtsp {
        class Connection {
        public:
            struct InterleavedBuffer {
                uint8_t channel;
                uint16_t size;
                uint8_t * data;
            };

            Connection( const char * source, const char * path );
            ~Connection();

            void open();
            void set( fd_set* rfds );
            int  fd() const {
                return m_session->fd();
            }
            int receive( fd_set* rfds, InterleavedBuffer * buffer );

            void pause();
            void scale( float sc );
            void resume( float position );
            void ping();

            void getParameter( const std::string & param );
            void setParameter( const std::string & param );

        private:
            static const char *kVersion;
            static const char *kOptions;
            static const char *kDescribe;
            static const char *kSetup;
            static const char *kPlay;
            static const char *kPause;
            static const char *kTeardown;
            static const char *kGetParameter;
            static const char *kSetParameter;
            static const char *kUserAgent;

            std::unique_ptr< BaseSession > m_session;

            int m_maxfd{-1};
            uint32_t m_cseq{0};
            std::string m_path;

            std::string m_protoline;
            std::vector< std::string > m_headers;
            std::set< std::string > m_options;
            std::mutex m_mutex;

            SessionDescription m_session_description;
            std::vector< std::shared_ptr< MediaSession > > m_media_sessions;
            std::string m_session_id;
            uint8_t m_max_channel;
            double m_range_begin{0.};
            double m_range_end{0.};

        private:
            std::string line();
            int resultCode( const std::string& );
            int response();
            void setRange( std::string& str );

            int askOptions();
            int askSdp();
            int askSetup( MediaSession & session/*, basic::Viewer* */ );
            void askPlay( float position = 0., float scale = 0. );
            void askTeardown(const std::string& id);
        };
    }  // namespace rtsp
}  // namespace dm