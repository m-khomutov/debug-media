#pragma once
#include "core/dmbasesession.h"
#include "core/dmbasereceiver.h"
/*#include "../seagull_decoder.h"
#include "seagull_socket.h"
#include "seagull_sdp.h"
*/
#include <memory>
#include <mutex>
#include <fstream>
#include <set>

namespace dm {
    namespace rtsp {

/*class MediaSession {
 public:
  MediaSession()
      : rtcp_socket_(rtp_socket_.port() + 1),
        dump_file_("/var/tmp/seagull.dump", std::ios::binary) {
  }
  ~MediaSession() {
     if (dump_file_.is_open()) {
        dump_file_.close();
     }
  }
  void setTransport(const char* line) throw( std::logic_error );
  void setID(const char* line);
  void setDecoder(basic::Viewer*);
  void FDSET(fd_set* rfds) {
    rtp_socket_.FDSET(rfds);
    rtcp_socket_.FDSET(rfds);
  }
  MediaDescription* getDescriptor() {
    return &media_description_;
  }
  const std::pair<uint16_t,uint16_t>& client_ports() {
    return client_ports_;
  }
  const std::pair<uint16_t,uint16_t>& server_ports() {
    return server_ports_;
  }
  std::string expected_client_ports() {
    return std::to_string(rtp_socket_.port()) +
           std::string("-") +
           std::to_string(rtcp_socket_.port());
  }
  int maxfd() {
    return std::max(rtp_socket_.fd(), rtcp_socket_.fd());
  }
  int getData(fd_set* rfds, uint8_t* buf, size_t bufsz, const MediaDescription * sdp, mp2ts::File* tsfile);

  const std::string& id()  { return id_;   }
  const std::string& mode(){ return mode_; }

  friend std::ostream& operator <<(std::ostream& out, const MediaSession&);

 private:
  void dump(const uint8_t* buf, size_t bufsz);

  rtp::Socket rtp_socket_;
  rtp::Socket rtcp_socket_;

  std::unique_ptr<Decoder> decoder_;

  MediaDescription media_description_;

  std::ofstream dump_file_;
  std::string protocol_;
  std::string type_;
  std::string mode_;
  std::string source_;
  std::string id_;

  uint32_t ssrc_;
  uint32_t timeout_;
  std::pair<uint16_t,uint16_t> client_ports_;
  std::pair<uint16_t,uint16_t> server_ports_;

  std::pair<uint16_t, uint16_t> getTransportPorts(const std::string& line);
};*/
        class Connection {
        public:
            Connection( const char * source, const char * path );
            ~Connection();

            void open( /*basic::Viewer* */ );
            void set( fd_set* rfds );
            int  maxfd() const {
                return m_maxfd;
            }
            int receive( fd_set* rfds, uint8_t * buf, size_t bufsz );

            int  pause( float pause_point );
            void scale( float sc );
            void resume( float position );
            void ping();

            void setPosition( int pos );
            int getParameter(std::string param);
            void setParameter(const char* param);

            const Range * range() const {
                return &m_range;
            }

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

            //basic::Buffer m_buffer;
            dm::Range m_range;
            std::unique_ptr< BaseSession > m_session;

            int m_maxfd{-1};
            uint32_t m_cseq{0};
            std::string m_path;

            std::string m_protoline;
            std::vector< std::string > m_headers;
            std::set< std::string > m_options;
            std::mutex m_mutex;

            //SessionDescription session_description_;
            //std::vector<MediaSession*> media_sessions_;

        private:
            std::string line();
            int resultCode( const std::string& );
            int response();
            void setRange( std::string& str );

            int askOptions();
            int askSdp();
            //int askSetup(MediaSession*, basic::Viewer*);
            //int askPlay(MediaSession*, int position = 0, float scale = .0);
            int askTeardown(const std::string& id);
        };
    }  // namespace rtsp
}  // namespace dm