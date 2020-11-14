#include "dmconnection.h"

#include <iostream>
#include <regex>
/*#include "../rtp/seagull_rtp.h"

#include <algorithm>
#include <fstream>

namespace seagull {
namespace rtsp {
std::ostream& operator <<(std::ostream& out, const MediaSession& session) {
  out << "[SESSION] " << session.id_ << " " << session.protocol_
            << ";client_port=" << session.rtp_socket_.port() << "-" << session.rtcp_socket_.port()
            << ";mode=" << session.mode_
            << ";server_port=" << session.server_ports_.first << "-" << session.server_ports_.second
            << ";source=" << session.source_ << ";ssrc=" << session.ssrc_
            << ";timeout=" << session.timeout_ << std::endl;

  return out;
}
}  // namespace rtsp
}  // namespace seagull

std::pair<uint16_t, uint16_t>
seagull::rtsp::MediaSession::getTransportPorts(const std::string& line) {
  std::string::size_type sz;

  uint16_t from = std::stoi(line, &sz);

  if (sz != std::string::npos) {
    uint16_t to = std::stoi(line.substr(sz + 1));

    return std::make_pair(from, to);
  }

  return std::make_pair(from, 0);
}

void seagull::rtsp::MediaSession::setTransport(const char* line) throw(std::logic_error) {
  const char* p1 = line, *p2;

//RTP/AVP/UDP;client_port=51471-51472;mode=PLAY;server_port=4096-4097;source=0.0.0.0;ssrc=3070180962;unicast
  while ((p2 = strchr(p1, ';'))) {
    std::string str(p1, p2 - p1);

    std::regex re("([[:alpha:]|_]+)=([[:print:]]+)");
    std::smatch m;

    if (std::regex_match(str, m, re)) {
      if (m[1] == "client_port") {
        client_ports_ = getTransportPorts(m[2].str());

        if (client_ports_.first != rtp_socket_.port() || client_ports_.second != rtcp_socket_.port()) {
          throw (std::logic_error (std::string("[RTP] transport client port error: ") + m[2].str()));
        }
      } else if (m[1] == "server_port") {
        server_ports_ = getTransportPorts(m[2].str());
      } else if (m[1] == "mode") {
        mode_ = m[2].str();
      } else if (m[1] == "source") {
        source_ = m[2].str();
      } else if (m[2] == "ssrc") {
        ssrc_ = std::stoll(m[2].str());
      }
    } else {
      str.find('/') != std::string::npos ? protocol_ = str : type_ = str;
    }
    p1 = p2 + 1;
  }
}

void seagull::rtsp::MediaSession::setID(const char* line) {
  const char* ptr = strchr(line, ';');

  id_ = ptr ? std::string(line, ptr - line) : std::string(line);

  if ((ptr = strstr(line, "timeout="))) timeout_ = strtol(ptr + 8, 0, 10);
}

void seagull::rtsp::MediaSession::setDecoder(basic::Viewer* viewer) {
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

int seagull::rtsp::MediaSession::getData(
    fd_set* rfds, uint8_t* buf, size_t bufsz, const MediaDescription* sdp, mp2ts::File * tsfile) {
  int ret = 0;

  if ((ret = rtp_socket_.getData(rfds, buf, bufsz)) > 0) {
    //dump(buf, bufsz);
    rtp::Header hdr(buf);
    //hdr.print( stderr );

    if (decoder_.get()) decoder_->setData(&hdr, ret - (hdr.m_payload - buf), sdp, tsfile);

    return ret;
  }

  return rtcp_socket_.getData(rfds, buf, bufsz);
}

void seagull::rtsp::MediaSession::dump(const uint8_t* buf, size_t bufsz) {
  //std::ofstream file("/var/tmp/seagull.dump", std::ios::binary);

  size_t sz = bufsz > 128 ? 128 : bufsz;
  if (dump_file_.is_open())
    dump_file_.write((char*)buf, sz);
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
: m_session( BaseSession::create( source ) ),
  m_path( path ) {}

dm::rtsp::Connection::~Connection() {

/*  bool asked_teardown = false;
  for (auto session : media_sessions_) {
    try {
      if (!asked_teardown) {
         askTeardown(session->id());
         asked_teardown = true;
      }
    } catch (const std::logic_error& ex) {
      std::cerr << "[TERADOWN exception] " << ex.what() << std::endl;
    }

    delete session;
  }*/
}

std::string dm::rtsp::Connection::line() {
    std::string ret;
    char ch;
    size_t sz = 0;
    while( (ch = m_session->getChar()) ) {
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

    //SDP* current_sdp = 0;
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
                    //current_sdp = &session_description_;
                    //current_sdp->set(s);
                    break;
                case 'm':
                    //media_sessions_.push_back(new MediaSession);
                    //current_sdp = media_sessions_.back()->getDescriptor();
                    //current_sdp->set(s);
                    break;
                //default:
                    //if (current_sdp) current_sdp->set(s);
                    ////setRange(s);
            }
        }
    }
    //session_description_.print(std::cerr);
    //for (auto session : media_sessions_) session->getDescriptor()->print(std::cerr);

    return ret;
}

/*int seagull::rtsp::Connection::askSetup(MediaSession* media, basic::Viewer* viewer) {
  int  ret = 0;

  {
    std::lock_guard<std::mutex> lk(rtsp_mutex_);

    std::string transport = std::string("Transport: ") + media->getDescriptor()->name.proto +
                            std::string("/UDP;unicast;client_port=") + media->expected_client_ports() +
                            std::string(";mode=\"PLAY\"");

    headers_.clear();
    headers_.push_back(std::string("CSeq: ") + std::to_string(++cSeq_));
    headers_.push_back(kUserAgent);
    headers_.push_back(transport);

    if( media->getDescriptor()->control.find("rtsp://") != std::string::npos) {
      protoline_ = std::string(kSetup) + media->getDescriptor()->control +
                   std::string(kVersion);
    } else {
      protoline_  = std::string(kSetup) +basic_session_->Address() + path_ + std::string("/") +
                    media->getDescriptor()->control + std::string(kVersion);
    }

    std::cerr << "< " << protoline_ << std::endl;
    for (auto hdr: headers_) std::cerr << "< " << hdr << std::endl;
    std::cerr << "\n\t* * *\n\n";

    basic_session_->PutRequest( protoline_.c_str(), headers_);

    while (true) {
      std::string s = line();

      if (s.empty()) break;
      if (!ret) ret = rescode(s);

      if (s.find("Transport: ") == 0) {
        try {
          media->setTransport(s.substr(11).c_str());
        } catch( const std::logic_error& err ) {
          std::cerr << "[MEDIA ERROR] " << err.what() << std::endl;
          return 404;
        }
      } else if (s.find("Session: ") == 0) {
        media->setID(s.substr(9).c_str());
      }

      std::cerr << "> " << s;
    }
  }

  if (ret == basic::Session::OK) { //&& media->mode() == "PLAY" )
    media->setDecoder(viewer);

    return askPlay(media);
  }

  return ret;
}

int seagull::rtsp::Connection::askPlay( MediaSession* media, int pos, float scale ) {
  std::lock_guard<std::mutex> lk(rtsp_mutex_);

  headers_.clear();
  headers_.push_back(std::string("CSeq: ") + std::to_string(++cSeq_));
  headers_.push_back(kUserAgent);
  headers_.push_back(std::string("Session: ") + media->id());
 
  if (pos) {
    headers_.push_back(std::string( "Range: npt=" ) + 
                      std::to_string( pos )        +
                      std::string( "-" ));
  }
  if (scale) {
    headers_.push_back(std::string("Scale: ") + std::to_string(scale));
  }
 
  protoline_ = std::string(kPlay) + std::string(" ") +
               basic_session_->Address() + path_ + std::string(" ") +
               std::string(kVersion);

  std::cerr << "< " << protoline_ << std::endl;
  for (auto hdr : headers_) std::cerr << "< " << hdr << std::endl;
  std::cerr << "\n\t* * *\n\n";

  basic_session_->PutRequest(protoline_.c_str(), headers_);

  return response();
}*/

int dm::rtsp::Connection::pause( float pause_point ) {
    std::lock_guard<std::mutex> lk( m_mutex );

    int ret = 0;
  /*for( auto session : m_media_sessions ) {
    if (!session->id().empty()) {
       headers_.clear();
       headers_.push_back(std::string("CSeq: ") + std::to_string(++cSeq_));
       headers_.push_back(kUserAgent);
       headers_.push_back(std::string( "Session: " ) + session->id());
 
       protoline_ = std::string(kPause) + std::string(" ") +
                    basic_session_->Address() + path_ + std::string(" ") +
                    std::string(kVersion);

       std::cerr << "< " << protoline_ << std::endl;
       for (auto hdr : headers_) std::cerr << "< " << hdr << std::endl;
       std::cerr << "\n\t* * *\n\n";

       basic_session_->PutRequest(protoline_.c_str(), headers_);

       ret = response();
     }
  }*/
    return ret;
}

int dm::rtsp::Connection::getParameter(std::string param) {
    std::lock_guard<std::mutex> lk( m_mutex );

  /*if (media_sessions_.empty()) return 0;

  std::string body = std::string("\r\n") + param;

  headers_.clear();
  headers_.push_back(std::string("CSeq: ") + std::to_string(++cSeq_));
  headers_.push_back(kUserAgent);
  headers_.push_back(std::string("Session: ") + media_sessions_[0]->id());
  headers_.push_back(std::string("Content-Length: ") + std::to_string(body.size()));
      
  protoline_ = std::string(kGetParameter) + std::string(" ") +
               basic_session_->Address() + path_ + std::string(" ") +
               std::string(kVersion);

  std::cerr << "< " <<  protoline_ << std::endl;
  for (auto hdr : headers_) std::cerr << "< " << hdr << std::endl;
  std::cerr << "< " << body << std::endl;
  std::cerr << "\n\t* * *\n\n";

  basic_session_->PutRequest(protoline_.c_str(), headers_, body.c_str());
*/
    return response();
}

void dm::rtsp::Connection::setParameter( const char * param ) {
  std::lock_guard< std::mutex > lk( m_mutex );

  /*if (media_sessions_.empty()) return;

  std::string body = std::string( "\r\n" ) + std::string( param );

  headers_.clear();
  headers_.push_back(std::string("CSeq: ") + std::to_string(++cSeq_));
  headers_.push_back(kUserAgent);
  headers_.push_back(std::string("Session: ") + media_sessions_[0]->id());
  headers_.push_back(std::string("Content-Length: ") + std::to_string(body.size()));
      
  protoline_ = std::string(kSetParameter) + std::string(" ") +
               basic_session_->Address() + path_ + std::string(" ") +
               std::string(kVersion);

  std::cerr << "< " << protoline_ << std::endl;
  for (auto hdr : headers_) std::cerr << "< " << hdr << std::endl;
  std::cerr << "< " << body << std::endl;
  std::cerr << "\n\t* * *\n\n";

  basic_session_->PutRequest(protoline_.c_str(), headers_, body.c_str());
  */
    response();
}

void dm::rtsp::Connection::setPosition( int pos ) {
    if( !pos )
        pos = m_range.first;
    //askPlay(media_sessions_[0], pos);
}

void dm::rtsp::Connection::scale( float sc ) {
    if( sc == .0 )
        sc = 1.0;
    //askPlay(media_sessions_[0], 0, sc);
}

void dm::rtsp::Connection::resume( float position ) {
  //askPlay(media_sessions_[0], position);
}

/*
void seagull::rtsp::Connection::ping() {
  std::lock_guard<std::mutex> lk(rtsp_mutex_);

   for( auto session : m_mediaSessions )
   {
      std::string session_id = std::string( "Session: " ) + session->id();

      m_headers.clear();

    headers_.push_back(std::string("CSeq: ") + std::to_string(++cSeq_));
    headers_.push_back( USER_AGENT );
    headers_.push_back( session_id );
 
      sprintf( m_protoLine, "%s %s%s %s", GET_PARAMETER, m_session->Address(), m_path.c_str(), VERSION );

      fprintf( stderr, "< %s\n", m_protoLine );
      for( auto header:m_headers ) fprintf( stderr, "< %s\n", header.c_str() );
      fprintf( stderr, "\n\t* * *\n\n" );

      m_buffer.clear();
      m_session->PutRequest( m_protoLine, m_headers );
   }
}
*/

int dm::rtsp::Connection::askTeardown(const std::string& id) {
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
    return response();
}

void dm::rtsp::Connection::open( /*basic::Viewer* viewer */ ) {
  if( askOptions() != BaseSession::OK )
      throw std::logic_error( "[RTSP] failed to get options" );
  if( askSdp() != BaseSession::OK )
      throw std::logic_error( "[RTSP] failed to get SDP" );

  /*for (auto media_session : media_sessions_) {
    if (askSetup(media_session, viewer) != basic::Session::OK) {
      std::cerr << "[RTSP] failed to setup for session ";
      media_session->getDescriptor()->print(std::cerr);
    } else {
      if (maxfd_ < media_session->maxfd()) maxfd_ = media_session->maxfd();
    }
    break;
  }

  if (maxfd_ < basic_session_->FD()) maxfd_ = basic_session_->FD();
*/}

void dm::rtsp::Connection::set( fd_set* rfds ) {
  m_session->set( rfds );
  //for (auto media_session : media_sessions_) media_session->FDSET(rfds);
}

int dm::rtsp::Connection::receive( fd_set* rfds, uint8_t* buf, size_t bufsz ) {
    int ret = 0;
    {
        std::lock_guard< std::mutex > lk( m_mutex );
        if( m_session->isset( rfds ) ) {
            char ch = m_session->getChar();
            std::cerr << ch;
        }
    }
    //for ( auto media_session : media_sessions_)
    //    if ((ret = media_session->getData(rfds, buf, bufsz, media_session->getDescriptor(), tsfile_ )))
    //        return ret;
    return ret;
}

void dm::rtsp::Connection::setRange( std::string & line ) {
    size_t pos_eq = line.find('=');
    size_t pos_ap = line.find('-');

    if( pos_eq != std::string::npos && pos_ap != std::string::npos ) {
        m_range.first = std::stof( line.substr( pos_eq + 1, pos_ap - pos_eq - 1 ) );
        if( pos_ap < line.size() - 3 )
            m_range.second = std::stof( line.substr( pos_ap + 1 ) );
    }
}
