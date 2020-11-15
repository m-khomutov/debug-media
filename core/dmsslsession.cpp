#include "dmsslsession.h"
#include <iostream>

dm::SslSession::SslSession( const char * source, const char * pemkey ) : BaseSession( source ) {
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSLeay_add_ssl_algorithms();

    m_ctx = std::unique_ptr< SSL_CTX, std::function< void(SSL_CTX*) > >(SSL_CTX_new( SSLv23_client_method() ), [](SSL_CTX*ctx){ SSL_CTX_free( ctx ); });
    if( !m_ctx )
        throw( std::logic_error( std::string( "[CTX] failed: " ) + std::string( ERR_reason_error_string( ERR_get_error() ) ) ) );
    if( !SSL_CTX_load_verify_locations( m_ctx.get(), pemkey, nullptr ) )
        throw( std::logic_error( std::string( "[PEM] failed to verify " ) + std::string( pemkey ) + std::string(": " ) + std::string( ERR_reason_error_string( ERR_get_error() ) ) ) );
}

void dm::SslSession::open() {
    const char * p = strstr( source().c_str(), "://" );

    p ? p += 3 : p = source().c_str();
    m_bio = std::unique_ptr< BIO, std::function< void(BIO*) > >( BIO_new_ssl_connect( m_ctx.get() ), [](BIO * b){ BIO_free_all( b ); });
    if( !m_bio )
        throw( std::logic_error( std::string( "[BIO] failed to make new connect: " ) + std::string( ERR_reason_error_string( ERR_get_error() ) ) ) );

    SSL * ssl;
    BIO_get_ssl ( m_bio.get(), &ssl );
    m_ssl = std::unique_ptr< SSL, std::function< void(SSL*) > >(ssl, [](SSL*s){ SSL_shutdown( s ); });

    SSL_set_mode( m_ssl.get(), SSL_MODE_AUTO_RETRY );
    BIO_set_conn_hostname( m_bio.get(), p );

    if( BIO_do_connect( m_bio.get() ) <= 0 )
        throw( std::logic_error( std::string( "[BIO] failed to do connect to: " ) + std::string( source() )  + std::string(": " ) + std::string( ERR_reason_error_string( ERR_get_error() ) ) ) );
    if( SSL_get_verify_result( m_ssl.get() ) != X509_V_OK )
        throw( std::logic_error( std::string( "[SSL] failed to verify connect to: " ) + std::string( source() ) + std::string(": " ) + std::string( ERR_reason_error_string( ERR_get_error() ) ) ) );
}

void dm::SslSession::close() {
    if( BIO_reset( m_bio.get() ) )
        std::cerr << "[BIO] failed to reset: " << ERR_reason_error_string( ERR_get_error() ) << std::endl;
    m_bio.reset();
}

int dm::SslSession::receive( uint8_t * buffer, size_t bufsz, bool whole ) {
   int ret = BIO_read( m_bio.get(), buffer, bufsz );
   if( ret == 0 ) {
    /* Handle closed connection */
   }
   else if( ret < 0 ) {
      if( !BIO_should_retry( m_bio.get() ) ) {
        /* Handle failed read here */
      }
   }
   return ret;
}

int dm::SslSession::send( const uint8_t * msg, size_t msgsz ) {
    int ret;
    if( (ret = BIO_write( m_bio.get(), msg, msgsz )) <= 0 ) {
        if( !BIO_should_retry( m_bio.get() ) ) {
        /* Handle failed write here */
        }
    }
    return ret;
}