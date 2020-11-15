#include "dmtcpsession.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>

#include <iostream>
#include <memory>

dm::TcpSession::TcpSession( const char * source ) : BaseSession( source ) {}

dm::TcpSession::~TcpSession () {
    close ();
}

void dm::TcpSession::open() {
    if( (m_connected == -1) && (m_connected = m_socket.connect( srcAddress() )) == -1 )
        throw ( std::logic_error( source() + std::string( " connect error: ") + std::string( strerror( errno ) ) ) );
}

int dm::TcpSession::fd() {
   return m_socket;
}

void dm::TcpSession::set( fd_set* rfds ) {
   m_socket.set( rfds );
}

bool dm::TcpSession::isset( fd_set* rfds ) {
   return m_socket.isset( rfds );
}

int dm::TcpSession::receive( uint8_t *buf, size_t sz, bool whole ) {
    if( m_connected == -1 )
        return -1;
    if( !whole )
        return m_socket.receive( (sockaddr*)&m_source, buf, sz );

    int ret = 0;
    while( ret < (int)sz ) {
        int rc;
        if( (rc = m_socket.receive( (sockaddr*)&m_source, buf + ret, sz - ret )) <= 0 )
            break;
        ret += rc;
    }
    return ret;
}

int dm::TcpSession::send( const uint8_t * msg, size_t msgsz ) {
   open();

   int ret;
   if( m_connected != -1 ) {
      if( (ret = m_socket.send( msg, msgsz )) == -1 )
          close ();
   }
   return ret;
}

void dm::TcpSession::close() {
   try {
      m_socket.close();
   }
   catch( const std::logic_error& err) {
       std::cerr << "[CLOSE err] " << err.what() << std::endl;
   }
   m_connected = -1;
}
