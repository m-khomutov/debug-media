#include "dmbasereceiver.h"
//#include "seagull_viewer.h"
//#include "../http/seagull_receiver.h"
//#include "../hls/seagull_receiver.h"
#include "rtsp/dmreceiver.h"
//#include "../mp2ts/seagull_receiver.h"

#include <regex>

const double dm::BaseReceiver::CACHE_SIZE_MSEC = 10.;

dm::BaseReceiver* dm::BaseReceiver::create( const char * source, const char* cert ) {
    /*if( strstr( url, "http://" )  == url ) {
        return new seagull::http::Receiver( viewer, url, cert );
    }
    else if( strstr( url, "https://" ) == url ) {
        return new seagull::hls::Receiver ( viewer, url, cert );
    }
    else if( strstr( url, "mp2t://" ) == url ) {
        return new seagull::mp2t::Receiver ( viewer, url, cert );
    }*/
    return new rtsp::Receiver( source, cert );
}

dm::BaseReceiver::BaseReceiver( const char * source, const char * cert )
   //: m_viewer(viewer),
: m_path( "/" ),
  m_cert( cert ? std::string( cert ) : std::string( "" ) ),
  m_ip( INADDR_ANY ),
  m_port( INADDR_ANY ) {

    enum {kWholeUrl, kProto, kHost, kPort, kPath};
    std::regex reg("(.*)://(.*):([[:d:]]{2,6})/(.*)"); // proto://host:port/path

    std::cmatch match_result;
    if( !std::regex_match( source, match_result, reg ) )
        throw std::logic_error(std::string("[invalid url proto://host:port/path] ") + std::string( source ) );

    m_url = match_result[kProto].str() + "://" + match_result[kHost].str() + ":" + match_result[kPort].str();
    m_path += match_result[kPath].str();
    m_port  = std::stoi(match_result[kPort]);

    hostent *hp = gethostbyname( match_result[kHost].str().c_str() );
    int i = 0;
    while( hp->h_addr_list[ i ] ) {
        m_ip = (*(in_addr*)(hp->h_addr_list[ i ])).s_addr;
        ++ i;
    }
}

double dm::BaseReceiver::duration () {
   return 0.; //m_viewer->cachedDuration();
}