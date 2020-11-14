#include "dmbasesession.h"
#include "dmtcpsession.h"
#include "dmsslsession.h"

#include <netdb.h>
#include <dirent.h>

#include <memory>
#include <regex>

//#include "seagull_ssl_session.h"

const char dm::BaseSession::kEol[ 2 ] = { 0x0D, 0x0A };

dm::BaseSession* dm::BaseSession::create( const char * source, const char* pemkey ) {
   if( strstr( source, "https://" ) == source )
       return new SslSession( source, pemkey );
    return new TcpSession( source );
}

dm::BaseSession::BaseSession( const char * source ) : m_source( source ) {
    enum {
        kWholeUrl, kProto, kHost, kPort
    };

    std::regex reg("(.*)://(.*):(.*)"); // proto://host:port
    std::cmatch match_result;
    if( !(std::regex_match( source, match_result, reg )) )
        throw std::logic_error( std::string("[invalid address proto://host:port] " ) + source );

    m_src_address.sin_family = AF_INET;
    m_src_address.sin_port   = htons(std::stoi( match_result[kPort].str() ) );
    hostent *hp = gethostbyname( match_result[kHost].str().c_str() );

    if( hp ) {
        int i = 0;
        while( hp->h_addr_list[ i ] ) {
            m_src_address.sin_addr.s_addr = (*(in_addr*)( hp->h_addr_list[ i ] ) ).s_addr;
            ++ i;
        }
    }
    if (m_src_address.sin_addr.s_addr == INADDR_NONE)
        throw std::logic_error(std::string("invalid host: " ) + match_result[kHost].str() );
}

char dm::BaseSession::getChar() {
   uint8_t ch;
   return receive( &ch, 1 ) == 1 ? ch : 0;
}

int dm::BaseSession::putLine( const char *line ) {
    size_t sz = strlen( line );
    if( !sz )
        return send( (const uint8_t*)kEol, sizeof( kEol ) );

    int rc{0};
    if( (rc = send( (const uint8_t*)line, sz )) > 0)
       rc += send( (const uint8_t*)kEol, sizeof( kEol ) );
    return rc;
}

int dm::BaseSession::putRequest( const char *protoline, const std::vector< std::string > & headers, const char * body ) {
   int rc = putLine( protoline );
   if( rc > 0 ) {
       for( auto header : headers ) {
           int s = putLine( header.c_str() );
           if( s <= 0 )
               return rc;
           rc += s;
       }
       if( body ) {
           int s = putLine( body );
           if( s <= 0 )
               return rc;
           rc += s;
       }
       int s = putLine( "" );
       if( s > 0 )
           rc += s;
   }
   return rc;
}

int dm::BaseSession::getStream( uint8_t* buffer, size_t bufsz ) {
   return receive( buffer, bufsz );
}