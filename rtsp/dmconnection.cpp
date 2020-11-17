#include "dmconnection.h"

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
const char * dm::rtsp::Connection::kVersion      = " RTSP/1.0";
const char * dm::rtsp::Connection::kOptions      = "OPTIONS ";
const char * dm::rtsp::Connection::kDescribe     = "DESCRIBE ";
const char * dm::rtsp::Connection::kSetup        = "SETUP ";
const char * dm::rtsp::Connection::kPlay         = "PLAY ";
const char * dm::rtsp::Connection::kPause        = "PAUSE ";
const char * dm::rtsp::Connection::kTeardown     = "TEARDOWN ";
const char * dm::rtsp::Connection::kGetParameter = "GET_PARAMETER ";
const char * dm::rtsp::Connection::kSetParameter = "SET_PARAMETER ";
const char * dm::rtsp::Connection::kUserAgent    = "User-Agent: DebugMedia Player/1.0.0";
//const char *seagull::rtsp::Connection::kUserAgent    = "User-Agent: Eltex STB (live555 based)";

dm::rtsp::Connection::Connection( const char * source, const char * path )
: m_session( BaseSession::create( source ) ),m_path( path ) {}

dm::rtsp::Connection::~Connection() {
    try {
        askTeardown( m_session_id );
    }
    catch( const std::logic_error& ex ) {
        std::cerr << "[TERADOWN exception] " << ex.what() << std::endl;
    }
}

std::string dm::rtsp::Connection::line() {
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

int dm::rtsp::Connection::resultCode( const std::string& line ) {
    size_t pos;
    if( (pos = line.find( &kVersion[1] )) != std::string::npos )
        return std::stoi( line.substr( pos + strlen(kVersion) ) );
    return -1;
}

int dm::rtsp::Connection::response() {
    int ret = 0;
    while( true ) {
        std::string s = line();
        if( s.empty() )
            break;
        if( !ret )
            ret = resultCode( s );
        if( s.find( "Range: npt=" ) != std::string::npos )
            setRange( s );
        std::cerr << "> " << s << std::endl;
    }
    return ret;
}

int dm::rtsp::Connection::askOptions() {
    std::lock_guard< std::mutex > lk( m_mutex );

    m_protoline = std::string(kOptions) + m_session->source() + std::string(kVersion);
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
        std::string s = line();
        if( s.empty() )
            break;
        if( !ret )
            ret = resultCode( s );
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

int dm::rtsp::Connection::askSdp() {
    std::lock_guard<std::mutex> lk( m_mutex );

    m_protoline = std::string(kDescribe) + m_session->source() + m_path + std::string(kVersion);
    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq ) );
    m_headers.push_back( "Accept: application/sdp" );
    m_headers.push_back( kUserAgent );

    std::cerr << "< " << m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers );

    SDP * current_sdp = nullptr;
    std::vector< MediaDescription > mds;
    int ret = 0;
    while( true ) {
        std::string s = line();
        if( s.empty() )
            break;
        std::cerr << "> " << s;
        if( !ret )
            ret = resultCode( s );

        if (s[1] == '=' && ret == 200 ) {
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
    for( auto d : mds )
        m_media_sessions.push_back( std::shared_ptr< MediaSession >( MediaSession::create( d, this ) ) );

    std::cerr << m_session_description << std::endl;
    for( auto session : m_media_sessions )
        std::cerr << *session->description() << std::endl;

    return ret;
}

int dm::rtsp::Connection::askSetup( MediaSession & media/*, basic::Viewer* viewer */ ) {
    int ret = 0;
    {
        std::lock_guard< std::mutex > lk( m_mutex );
        std::string transport = std::string("Transport: ") + media.description()->name.protocol() + std::string("/TCP;unicast;mode=\"PLAY\"");

        m_headers.clear();
        m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
        m_headers.push_back( kUserAgent );
        m_headers.push_back( transport );
        if( !m_session_id.empty() )
            m_headers.push_back( std::string( "Session: ") + m_session_id );

        if( media.description()->control.find("rtsp://") != std::string::npos) {
            m_protoline = std::string(kSetup) + media.description()->control + std::string(kVersion);
        }
        else
            m_protoline = std::string(kSetup) + m_session->source() + m_path + std::string("/") + media.description()->control + std::string(kVersion);

        std::cerr << "< " << m_protoline << std::endl;
        for( auto hdr: m_headers )
            std::cerr << "< " << hdr << std::endl;
        std::cerr << "\n\t* * *\n\n";

        m_session->putRequest( m_protoline.c_str(), m_headers );
        while( true ) {
            std::string s = line();
            if( s.empty() )
                break;
            if( !ret )
                ret = resultCode( s );

            if( s.find("Transport: " ) == 0 ) {
                try {
                    media.setTransport( s.substr(11).c_str() );
                } catch( const std::logic_error& err ) {
                    std::cerr << "[MEDIA ERROR] " << err.what() << std::endl;
                    return 404;
                }
            } else if( s.find("Session: ") == 0 ) {
                media.setId( s.substr(9).c_str() );
            }
            std::cerr << "> " << s;
        }
    }

  /*if (ret == basic::Session::OK) { //&& media->mode() == "PLAY" )
    media->setDecoder(viewer);
  }*/
  return ret;
}

void dm::rtsp::Connection::askPlay( float pos, float scale ) {
    std::lock_guard< std::mutex > lk( m_mutex );

    m_headers.clear();
    if( pos )
        m_headers.push_back( std::string( "Range: npt=" ) + std::to_string( pos ) + std::string( "-" ) );
    if( scale )
        m_headers.push_back( std::string("Scale: ") + std::to_string(scale) );
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back( kUserAgent);
    m_headers.push_back( std::string("Session: ") + m_session_id );

    m_protoline = std::string(kPlay) + std::string(" ") + m_session->source() + m_path + std::string(" ") + std::string(kVersion);
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
 
        m_protoline = std::string(kPause) + std::string(" ") + m_session->source() + m_path + std::string(" ") + std::string(kVersion);
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
    m_headers.push_back( std::string("Content-Length: ") + std::to_string(body.size()) );
      
    m_protoline = std::string(kGetParameter) + std::string(" ") + m_session->source() + m_path + std::string(" ") + std::string(kVersion);
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
    m_headers.push_back( std::string("Content-Length: ") + std::to_string(body.size()));

    m_protoline = std::string(kSetParameter) + std::string(" ") + m_session->source() + m_path + std::string(" ") + std::string(kVersion);
    std::cerr << "< " << m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "< " << body << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers, body.c_str() );
}

void dm::rtsp::Connection::scale( float sc ) {
    if( sc == .0 )
        sc = 1.0;
    askPlay( 0, sc );
}

void dm::rtsp::Connection::resume( float position ) {
    askPlay( position );
}

void dm::rtsp::Connection::ping() {
    std::lock_guard<std::mutex> lk( m_mutex );

    if( m_session_id.empty() )
        return;
    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back( kUserAgent );
    m_headers.push_back( std::string("Session: ") + m_session_id );

    m_protoline = std::string(kGetParameter) + std::string(" ") + m_session->source() + m_path + std::string(" ") + std::string(kVersion);
    std::cerr << "< " <<  m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers );
}

void dm::rtsp::Connection::askTeardown(const std::string& id) {
    std::lock_guard< std::mutex > lk( m_mutex );

    m_headers.clear();
    m_headers.push_back( std::string("CSeq: ") + std::to_string(++m_cseq) );
    m_headers.push_back(kUserAgent);
    m_headers.push_back(std::string("Session: ") + id);
 
    m_protoline = std::string(kTeardown) + std::string(" ") + m_session->source() + m_path + std::string(" ") + std::string(kVersion);

    std::cerr << "< " << m_protoline << std::endl;
    for( auto hdr : m_headers )
        std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    m_session->putRequest( m_protoline.c_str(), m_headers );
}

void dm::rtsp::Connection::open( /*basic::Viewer* viewer */ ) {
    if( askOptions() != BaseSession::OK )
        throw std::logic_error( "[RTSP] failed to get options" );
    if( askSdp() != BaseSession::OK )
        throw std::logic_error( "[RTSP] failed to get SDP" );
    for( auto ms : m_media_sessions ) {
        if( askSetup( *ms ) != BaseSession::OK ) {
            std::cerr << "[RTSP] failed to setup for session ";
            return;
        } else {
            m_session_id = ms->id();
        }
    }
    askPlay();
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
                        buffer->channel = c;
                        state = kWaitingSize0;
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

void dm::rtsp::Connection::setRange( std::string & line ) {
    size_t pos_eq = line.find('=');
    size_t pos_ap = line.find('-');

    if( pos_eq != std::string::npos && pos_ap != std::string::npos ) {
        m_range_begin = std::stof( line.substr( pos_eq + 1, pos_ap - pos_eq - 1 ) );
        if( pos_ap < line.size() - 3 )
            m_range_end = std::stof( line.substr( pos_ap + 1 ) );
    }
}
