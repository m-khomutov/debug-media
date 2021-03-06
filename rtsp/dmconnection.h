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
            struct DigestAuthentication {
                std::string realm;
                std::string nonce;
                std::string stale;

                void parse( const char * header );

            };
            struct BasicAuthentication {
                std::string realm;

                void parse( const char * header );
            };

            Connection( const std::string & source, const std::string & path, const std::string & user, const std::string & passwd );
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
            std::string m_url;
            std::string m_path;
            std::string m_user;
            std::string m_password;

            std::string m_protoline;
            std::vector< std::string > m_headers;
            std::set< std::string > m_options;
            std::mutex m_mutex;

            DigestAuthentication m_digest_authentication;
            BasicAuthentication m_basic_authentication;

            SessionDescription m_session_description;
            std::vector< std::shared_ptr< MediaSession > > m_media_sessions;
            std::string m_session_id;
            std::string m_autholization_header;
            uint8_t m_max_channel;
            double m_range_begin{0.};
            double m_range_end{0.};

        private:
            void f_parse_authenticated_url( const char * url );
            void f_parse_url( const char * url );

            std::string f_line();
            int f_result_code( const std::string& );
            int f_response();
            void f_set_range( std::string& str );
            void f_prepare_basic_authorization();
            std::string f_prepare_digest_authorization( const std::string & method, const std::string & url );

            int f_ask_options();
            int f_ask_sdp();
            int f_ask_setup( MediaSession & session/*, basic::Viewer* */ );
            void f_ask_play( float position = 0., float scale = 0. );
            void f_ask_teardown(const std::string& id);
        };
    }  // namespace rtsp
}  // namespace dm