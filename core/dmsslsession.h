#pragma once
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include "dmbasesession.h"

#include <memory>
#include <functional>
#include <stdexcept>

namespace dm {
    class SslSession : public BaseSession {
    public:
        SslSession( const char * source, const char * pemkey );

        void open() override;
        void close() override;

        int receive( uint8_t * buffer, size_t buffersz, bool whole ) override;
        int send( const uint8_t * msg, size_t msgsz ) override;

    public:
        std::unique_ptr< BIO, std::function< void(BIO*) > > m_bio;
        std::unique_ptr< SSL, std::function< void(SSL*) > > m_ssl;
        std::unique_ptr< SSL_CTX, std::function< void(SSL_CTX*) > > m_ctx;
};
}  // namespace dm
