#include "dmbasereceiver.h"
#include "rtsp/dmreceiver.h"

#include <regex>

dm::BaseReceiver* dm::BaseReceiver::create( const char * source, const char* cert ) {
    /*if( strstr( url, "http://" )  == url ) {
        return new dm::http::Receiver( viewer, url, cert );
    }
    else if( strstr( url, "https://" ) == url ) {
        return new sdm::hls::Receiver ( viewer, url, cert );
    }
    else if( strstr( url, "mp2t://" ) == url ) {
        return new dm::mp2t::Receiver ( viewer, url, cert );
    }*/
    return new rtsp::Receiver( source, cert );
}

dm::BaseReceiver::BaseReceiver( const char * source, const char * cert )
: m_path( "/" ),
  m_cert( cert ? std::string( cert ) : std::string( "" ) ),
  m_ip( INADDR_ANY ),
  m_port( INADDR_ANY ) {

    if( strchr( source, '@' ) ) {
        f_parse_authenticated_url( source );
    }
    else
        f_parse_url( source );

    hostent *hp = gethostbyname( m_host.c_str() );
    int i = 0;
    while( hp->h_addr_list[ i ] ) {
        m_ip = (*(in_addr*)(hp->h_addr_list[ i ])).s_addr;
        ++ i;
    }
}

void dm::BaseReceiver::f_parse_authenticated_url( const char * url ) {
    enum {kWholeUrl, kProto, kUser, kPassword, kHost, kPort, kPath};

    std::regex reg("([a-z]*)://([[:graph:]]*):([[:graph:]]*)@([0-9\\.]*)([:0-9]*)/(.*)"); // proto://user:password@host[:port]/path
    std::cmatch res;
    if( !std::regex_match( url, res, reg ) )
        throw std::logic_error(std::string("[invalid url proto://user:password@host:port/path] ") + std::string( url ) );

    if( !res[kPort].str().empty() )
        m_port  = strtol(res[kPort].str().c_str()+1, nullptr, 10 );
    if( !m_port )
        m_port = 554;
    m_host = res[kHost].str();
    m_user = res[kUser].str();
    m_password = res[kPassword].str();
    m_url = res[kProto].str() + "://" + m_host + ":" + std::to_string( m_port );
    m_path += res[kPath].str();
}

void dm::BaseReceiver::f_parse_url( const char * url ) {
    enum {kWholeUrl, kProto, kHost, kPort, kPath};
    std::regex reg("([a-z]*)://([0-9\\.]*)([:0-9]*)/(.*)"); // proto://host[:port]/path

    std::cmatch res;
    if( !std::regex_match( url, res, reg ) )
        throw std::logic_error(std::string("[invalid url proto://host:port/path] ") + std::string( url ) );

    if( !res[kPort].str().empty() )
        m_port  = strtol(res[kPort].str().c_str()+1, nullptr, 10 );
    if( !m_port )
        m_port = 554;
    m_host = res[kHost].str();
    m_url = res[kProto].str() + "://" + m_host + ":" + std::to_string( m_port );
    m_path += res[kPath].str();
}
