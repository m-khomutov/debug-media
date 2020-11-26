#include "dmconnection.h"
#include "core/dmbase64.h"

extern "C" {
#include <openssl/md5.h>
};

#include <iostream>
#include <regex>
#include <algorithm>

/*void seagull::rtsp::MediaSession::setDecoder(basic::Viewer* viewer) {
  if (decoder_.get()) return;

  for (auto attrib : media_description_.attributes) {
    if (attrib.find("rtpmap") != std::string::npos) {
      AVCodecID id = AV_CODEC_ID_NONE;

      if (attrib.find("MPV") != std::string::npos) {
        id = AV_CODEC_ID_MPEG2VIDEO;
      } else if (attrib.find("H264") != std::string::npos) {
        id = AV_CODEC_ID_H264;
      } else if (attrib.find("JPEG") != std::string::npos) {
        id = AV_CODEC_ID_MJPEG;
      } else if (attrib.find("MPA")  != std::string::npos) {
        id = AV_CODEC_ID_MP3;
      } else if (attrib.find("mpeg4-generic" ) != std::string::npos) {
        id = AV_CODEC_ID_AAC;
      }

      if (id != AV_CODEC_ID_NONE) {
        decoder_.reset(new Decoder(id, viewer, attrib, media_description_.name.format));
        if (id == AV_CODEC_ID_AAC) decoder_->setAacMode(media_description_.find("mode="));
      }

      return;
    }
  }
}
*/
namespace {
    std::string md5sum( const uint8_t *buffer, size_t blen ) {
        static const char hexDigits[ 17 ] = "0123456789abcdef";
        unsigned char digest[ MD5_DIGEST_LENGTH ];
        char digest_str[ 2 * MD5_DIGEST_LENGTH + 1 ];

        MD5( (const unsigned char *)buffer, blen, digest );
        for( int i(0); i < MD5_DIGEST_LENGTH; i++ ) {
            digest_str[ i * 2 ] = hexDigits[ ( digest[ i ] >> 4 ) & 0xF ];
            digest_str[ i * 2 + 1 ] = hexDigits[ digest[ i ] & 0xF ];
        }
        digest_str[ MD5_DIGEST_LENGTH * 2 ] = '\0';
        return std::string( digest_str );
    }
}  // namespace

void dm::rtsp::Connection::DigestAuthentication::parse( const char *header ) {
    const char * ptr = strstr( header, "realm=\"" );
    if( ptr ) {
        const char * p = strchr( ptr+8, '"' );
        if( p ) {
            realm.assign( ptr + 7, p - (ptr + 7) );
            ptr = strstr( p, "nonce=\"" );
            if( ptr ) {
                p = strchr( ptr+8, '"' );
                if( p ) {
                    nonce.assign( ptr + 7, p - (ptr + 7) );
                    ptr = strstr( p, "stale=\"" );
                    if( ptr ) {
                        p = strchr( ptr+8, '"' );
                        if( p )
                            stale.assign( ptr + 7, p - (ptr + 7) );
                    }
                }
            }
        }
    }
}
void dm::rtsp::Connection::BasicAuthentication::parse( const char *header ) {
    const char * ptr = strstr( header, "realm=\"" );
    if( ptr ) {
        const char * p = strchr( ptr+8, '"' );
        if( p )
            realm.assign( ptr + 7, p - (ptr + 7) );
    }
}



const char * dm::rtsp::Connection::kVersion      = "RTSP/1.0";
const char * dm::rtsp::Connection::kOptions      = "OPTIONS";
const char * dm::rtsp::Connection::kDescribe     = "DESCRIBE";
const char * dm::rtsp::Connection::kSetup        = "SETUP";
const char * dm::rtsp::Connection::kPlay         = "PLAY";
const char * dm::rtsp::Connection::kPause        = "PAUSE";
const char * dm::rtsp::Connection::kTeardown     = "TEARDOWN";
const char * dm::rtsp::Connection::kGetParameter = "GET_PARAMETER";
const char * dm::rtsp::Connection::kSetParameter = "SET_PARAMETER";
const char * dm::rtsp::Connection::kUserAgent    = "User-Agent: DebugMedia Player/1.0.0";
//const char *seagull::rtsp::Connection::kUserAgent    = "User-Agent: Eltex STB (live555 based)";

dm::rtsp::Connection::Connection( const std::string & source, const std::string & path, const std::string & user, const std::string & passwd )
: m_session( BaseSession::create( source.c_str() ) ),m_url( source + path ),m_path( path ),m_user( user ),m_password( passwd ) {}

dm::rtsp::Connection::~Connection() {
    try {
        f_ask_teardown( m_session_id );
    }
    catch( const std::logic_error& ex ) {
        std::cerr << "[TERADOWN exception] " << ex.what() << std::endl;
    }
}

std::string dm::rtsp::Connection::f_line() {
    std::string ret;
    char ch;
    size_t sz = 0;
    while( m_session->getChar( &ch ) && ch != 0 ) {
        ret += ch;
        ++sz;
        if( sz >= 2 && ret[sz - 2] == 0x0d && ret[sz - 1] == 0x0a )
            return ret;
    }
    return ret;
}

int dm::rtsp::Connection::f_result_code( const std::string& line ) {
    size_t pos;
    if( (pos = line.find( kVersion )) != std::string::npos )
        return std::stoi( line.substr( pos + strlen(kVersion) + 1 ) );
    return -1;
}

int dm::rtsp::Connection::f_response() {
    int ret = 0;
    while( true ) {
        std::string s = f_line();
        if( s.empty() )
            break;
        size_t  p = s.find( kVersion );
        if( p != std::string::npos ) {
            s = s.substr( p );
            if( !ret )
                ret = f_result_code( s );
            if( s.find( "Range: npt=" ) != std::string::npos )
                f_set_range( s );
            std::cerr << "> " << s;
        }
        else if( ret && ::isalpha( s[0] ) )
            std::cerr << "> " << s;
    }
    return ret;
}

int dm::rtsp::Connection::f_ask_options() {
    std::lock_guard< std::mutex > lk( m_mutex );

    m_protoline = std::string(kOptions) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);
    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back( kUserAgent );

    std::cerr << "< " << m_protoline << std::endl;
    for (auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_options.clear();
    m_session->putRequest( m_protoline.c_str(), m_headers );

    int ret = 0;
    while( true ) {
        std::string s = f_line();
        if( s.empty() )
            break;
        if( !ret )
            ret = f_result_code( s );
        std::cerr << "> " << s;

        if( s.find( "Public: " ) != std::string::npos ) {
            std::regex re("[[:s:]]([[:upper:]|_]+)");
            std::smatch m;
            while (std::regex_search(s, m, re)) {
                m_options.insert( m[1].str() );
                s = m.suffix().str();
            }
        }
    }

    if( m_options.find("DESCRIBE") == m_options.end() || m_options.find("SETUP") == m_options.end() || m_options.find("PLAY") == m_options.end() )
        ret = 405;
    return ret;
}

int dm::rtsp::Connection::f_ask_sdp() {
    std::lock_guard<std::mutex> lk( m_mutex );

    m_protoline = std::string(kDescribe) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);
    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq ) );
    m_headers.push_back( "Accept: application/sdp" );
    m_headers.push_back( kUserAgent );
    if( !m_digest_authentication.realm.empty() ) {
        m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kDescribe, m_url ) );
    }
    else if( !m_autholization_header.empty() )
        m_headers.push_back( std::string("Authorization: ") + m_autholization_header );

    std::cerr << "< " << m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers );

    SDP * current_sdp = nullptr;
    std::vector< MediaDescription > mds;
    int ret = 0;
    while( true ) {
        std::string s = f_line();
        if( s.empty() )
            break;
        std::cerr << "> " << s;
        if( !ret )
            ret = f_result_code( s );
        if( ret == BaseSession::Unauthorized ) {
            size_t pos;
            if( s.find( "WWW-Authenticate: Digest " ) != std::string::npos )
                m_digest_authentication.parse( s.c_str() );
            if( s.find( "WWW-Authenticate: Basic " ) != std::string::npos )
                m_basic_authentication.parse( s.c_str() );
        }
        if (s[1] == '=' && ret == BaseSession::OK ) {
            switch( s[0] ) {
                case 'v':
                    current_sdp = &m_session_description;
                    current_sdp->parse( s );
                    break;
                case 'm':
                    mds.push_back( MediaDescription() );
                    current_sdp = &mds.back();
                    current_sdp->parse( s );
                    break;
                default:
                    if( current_sdp )
                        current_sdp->parse( s );
            }
        }
    }
    for( auto d : mds ) {
        MediaSession * ms = MediaSession::create( d, this );
        if( ms )
            m_media_sessions.push_back( std::shared_ptr< MediaSession >( ms ) );
    }

    if( ret == BaseSession::OK ) {
        std::cerr << m_session_description << std::endl;
        for( auto session : m_media_sessions )
            std::cerr << *session->description() << std::endl;
    }
    return ret;
}

int dm::rtsp::Connection::f_ask_setup( MediaSession & media ) {
    int ret = 0;
    {
        std::lock_guard< std::mutex > lk( m_mutex );
        std::string transport = std::string("Transport: ") + media.description()->name.protocol() + std::string("/TCP;unicast;mode=\"PLAY\"");

        std::string url = media.description()->control.find("rtsp://") != std::string::npos ? media.description()->control : m_url + std::string("/") + media.description()->control;
        m_protoline = std::string(kSetup) + std::string(" ") + url + std::string(" ") + std::string(kVersion);

        m_headers.clear();
        m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
        m_headers.push_back( kUserAgent );
        m_headers.push_back( transport );
        if( !m_session_id.empty() )
            m_headers.push_back( std::string( "Session: ") + m_session_id );
        if( !m_digest_authentication.realm.empty() ) {
            m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kSetup, url ) );
        }
        if( !m_autholization_header.empty() )
            m_headers.push_back( std::string("Authorization: ") + m_autholization_header );

        std::cerr << "< " << m_protoline << std::endl;
        for( auto hdr: m_headers )
            std::cerr << "< " << hdr << std::endl;
        std::cerr << "\n\t* * *\n\n";

        m_session->putRequest( m_protoline.c_str(), m_headers );
        while( true ) {
            std::string s = f_line();
            if( s.empty() )
                break;
            if( !ret )
                ret = f_result_code( s );

            if( s.find("Transport: " ) == 0 ) {
                media.setTransport( s.substr(11).c_str() );
                if( media.channel() > m_max_channel )
                    m_max_channel = media.channel();
            } else if( s.find("Session: ") == 0 ) {
                media.setId( s.substr(9).c_str() );
            }
            std::cerr << "> " << s;
        }
    }
    return ret;
}

void dm::rtsp::Connection::f_ask_play( float pos, float scale ) {
    std::lock_guard< std::mutex > lk( m_mutex );

    m_headers.clear();
    if( pos )
        m_headers.push_back( std::string( "Range: npt=" ) + std::to_string( pos ) + std::string( "-" ) );
    if( scale )
        m_headers.push_back( std::string("Scale: ") + std::to_string(scale) );
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back( kUserAgent);
    m_headers.push_back( std::string("Session: ") + m_session_id );
    if( !m_digest_authentication.realm.empty() ) {
        m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kPlay, m_url ) );
    }
    else if( !m_autholization_header.empty() )
        m_headers.push_back( std::string("Authorization: ") + m_autholization_header );

    m_protoline = std::string(kPlay) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);
    std::cerr << "< " << m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers );
}

void dm::rtsp::Connection::pause() {
    std::lock_guard<std::mutex> lk( m_mutex );

    int ret = 0;
    if( !m_session_id.empty() ) {
        m_headers.clear();
        m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
        m_headers.push_back( kUserAgent );
        m_headers.push_back(std::string( "Session: " ) + m_session_id );
        if( !m_digest_authentication.realm.empty() ) {
            m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kPause, m_url ) );
        }
        else if( !m_autholization_header.empty() )
            m_headers.push_back( std::string("Authorization: ") + m_autholization_header );

        m_protoline = std::string(kPause) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);
        std::cerr << "< " << m_protoline << std::endl;
        for( auto hdr : m_headers )
            std::cerr << "< " << hdr << std::endl;
        std::cerr << "\n\t* * *\n\n";

        m_session->putRequest( m_protoline.c_str(), m_headers );
    }
}

void dm::rtsp::Connection::getParameter( const std::string & param ) {
    std::lock_guard<std::mutex> lk( m_mutex );

    if( m_session_id.empty() )
        return;
    std::string body = std::string("\r\n") + param;
    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back( kUserAgent );
    m_headers.push_back( std::string("Session: ") + m_session_id );
    if( !m_digest_authentication.realm.empty() ) {
        m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kGetParameter, m_url ) );
    }
    else if( !m_autholization_header.empty() )
        m_headers.push_back( std::string("Authorization: ") + m_autholization_header );
    m_headers.push_back( std::string("Content-Length: ") + std::to_string(body.size()) );
      
    m_protoline = std::string(kGetParameter) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);
    std::cerr << "< " <<  m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "< " << body << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers, body.c_str() );
}

void dm::rtsp::Connection::setParameter( const std::string & param ) {
  std::lock_guard< std::mutex > lk( m_mutex );

    if( m_session_id.empty() )
        return;
    std::string body = std::string( "\r\n" ) + param;

    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back( kUserAgent );
    m_headers.push_back( std::string("Session: ") + m_session_id );
    if( !m_digest_authentication.realm.empty() ) {
        m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kSetParameter, m_url ) );
    }
    else if( !m_autholization_header.empty() )
        m_headers.push_back( std::string("Authorization: ") + m_autholization_header );
    m_headers.push_back( std::string("Content-Length: ") + std::to_string(body.size()));

    m_protoline = std::string(kSetParameter) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);
    std::cerr << "< " << m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "< " << body << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers, body.c_str() );
}

void dm::rtsp::Connection::scale( float sc ) {
    f_ask_play( 0, sc == 0. ? 1. : sc );
}

void dm::rtsp::Connection::resume( float position ) {
    f_ask_play( position );
}

void dm::rtsp::Connection::ping() {
    std::lock_guard<std::mutex> lk( m_mutex );

    if( m_session_id.empty() )
        return;
    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back( kUserAgent );
    m_headers.push_back( std::string("Session: ") + m_session_id );
    if( !m_digest_authentication.realm.empty() ) {
        m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kGetParameter, m_url ) );
    }
    else if( !m_autholization_header.empty() )
        m_headers.push_back( std::string("Authorization: ") + m_autholization_header );

    m_protoline = std::string(kGetParameter) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);
    std::cerr << "< " <<  m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers );
}

void dm::rtsp::Connection::f_ask_teardown( const std::string& id ) {
    std::lock_guard< std::mutex > lk( m_mutex );

    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back(kUserAgent);
    m_headers.push_back(std::string("Session: ") + id);
    if( !m_digest_authentication.realm.empty() ) {
        m_headers.push_back( std::string("Authorization: ") + f_prepare_digest_authorization( kTeardown, m_url ) );
    }
    else if( !m_autholization_header.empty() )
        m_headers.push_back( std::string("Authorization: ") + m_autholization_header );

    m_protoline = std::string(kTeardown) + std::string(" ") + m_url + std::string(" ") + std::string(kVersion);

    std::cerr << "< " << m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers );
    f_response();
}

void dm::rtsp::Connection::open() {
    if( f_ask_options() != BaseSession::OK )
        throw std::logic_error( "[RTSP] failed to get options" );
    int rc;
    if( (rc = f_ask_sdp()) != BaseSession::OK ) {
        if( rc == BaseSession::Unauthorized ) {
            f_prepare_basic_authorization();
            if( f_ask_sdp() != BaseSession::OK )
                throw std::logic_error( "[RTSP] failed to get SDP" );
        }
        else
            throw std::logic_error( "[RTSP] failed to get SDP" );
    }
    for( auto ms : m_media_sessions ) {
        if( f_ask_setup( *ms ) != BaseSession::OK ) {
            std::cerr << "[RTSP] failed to setup for session ";
            return;
        } else {
            m_session_id = ms->id();
        }
    }
    f_ask_play();
}

void dm::rtsp::Connection::set( fd_set* rfds ) {
  m_session->set( rfds );
}

int dm::rtsp::Connection::receive( fd_set* rfds, InterleavedBuffer * buffer ) {
    int ret = 0;
    {
        std::lock_guard< std::mutex > lk( m_mutex );
        if( m_session->isset( rfds ) ) {
            enum State {kWaitingSign, kWaitingChannel, kWaitingSize0, kWaitingSize1, kWaitingPacket };

            State state = kWaitingSign;
            char c;
            while( state != kWaitingPacket && m_session->getChar( &c ) ) {
                switch( state ) {
                    case kWaitingSign:
                        if( c == '$' )
                            state = kWaitingChannel;
                        else
                            std::cerr << c;
                        break;
                    case kWaitingChannel:
                        if( uint8_t(c) > m_max_channel ) {
                            state = kWaitingSign;
                        }
                        else {
                            buffer->channel = c;
                            state = kWaitingSize0;
                        }
                        break;
                    case kWaitingSize0:
                        buffer->size = (uint8_t(c) << 8);
                        state = kWaitingSize1;
                        break;
                    case kWaitingSize1:
                        buffer->size |= uint8_t(c);
                        state = kWaitingPacket;
                        break;
                }
            }
            if( state == kWaitingPacket ) {
                ret = m_session->receive( buffer->data, buffer->size, true );
                for( auto session : m_media_sessions ) {
                    if( session->channel() == buffer->channel ) {
                        session->receiveInterleaved( buffer->data, buffer->size );
                        break;
                    }
                }
            }
        }
    }
    return ret;
}

void dm::rtsp::Connection::f_set_range( std::string & line ) {
    size_t pos_eq = line.find('=');
    size_t pos_ap = line.find('-');

    if( pos_eq != std::string::npos && pos_ap != std::string::npos ) {
        m_range_begin = std::stof( line.substr( pos_eq + 1, pos_ap - pos_eq - 1 ) );
        if( pos_ap < line.size() - 3 )
            m_range_end = std::stof( line.substr( pos_ap + 1 ) );
    }
}

void dm::rtsp::Connection::f_prepare_basic_authorization() {
    if( !m_basic_authentication.realm.empty() ) {
        std::string auth = m_user + ":" + m_password;
        m_autholization_header = std::string("Basic ") + Base64( (const uint8_t*)auth.c_str(), auth.size() ).enc();
    }

}

std::string dm::rtsp::Connection::f_prepare_digest_authorization( const std::string & method, const std::string & url ) {
    if( !m_digest_authentication.realm.empty() ) {
        std::string HA1Content = m_user + ":" + m_digest_authentication.realm + ":" + m_password;
        std::string HA1Value = md5sum( (const uint8_t *)HA1Content.data(),HA1Content.size() );
        std::string HA2Content = method + ":" + url;
        std::string HA2Value = md5sum( (const uint8_t *)HA2Content.data(), HA2Content.size() );

        std::string responseContent = HA1Value + ":" + m_digest_authentication.nonce + ":" + HA2Value;
        std::string responseValue = md5sum( (const uint8_t *)responseContent.data(), responseContent.size() );

        return std::move( std::string("Digest username=\"")+m_user+"\", realm=\""+m_digest_authentication.realm+"\", nonce=\""+
                m_digest_authentication.nonce+"\", uri=\""+url+"\", response=\""+responseValue+"\"" );
    }
}
