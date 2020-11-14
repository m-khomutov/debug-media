//
// Created by mkh on 13.11.2020.
//

#include "dmtcpsocket.h"

#include <fcntl.h>
#include <netinet/tcp.h>

dm::TcpSocket::TcpSocket() {
    f_open();
}
dm::TcpSocket::~TcpSocket() {
    close();
}
dm::TcpSocket::operator int() {
    return m_fd;
}

void dm::TcpSocket::set( fd_set* rfds ) {
    FD_SET( m_fd, rfds );
}
bool dm::TcpSocket::isset( fd_set* rfds ) {
    return FD_ISSET( m_fd, rfds );
}

int dm::TcpSocket::connect( sockaddr * to ) {
    int rc = ::connect( m_fd, to, sizeof( sockaddr_in ) );

    if( rc == -1 && errno == EINPROGRESS ) {
        fd_set wfds;
        timeval tv = { 5, 0 };
        FD_ZERO( &wfds );
        FD_SET( m_fd, &wfds );
        if( (rc = select( m_fd + 1, NULL, &wfds, NULL, &tv )) < 0 && errno != EINTR ) {
            return rc;
        }
        else if( (rc > 0 ) ) {
            socklen_t slen = sizeof( int );
            int nok;
            if( getsockopt( m_fd, SOL_SOCKET, SO_ERROR, (void*)(&nok), &slen ) < 0 || nok )
                return -1;
        }
    }
    return rc;
}

int dm::TcpSocket::send( const uint8_t * msg, size_t msgsz ) {
    size_t rc = 0, wait_periods = 3000;
    while( rc < msgsz ) {
        int s = ::send( m_fd, &msg[ rc ], msgsz - rc, 0 );
        if( s > 0 ) {
            rc += s;
            wait_periods = 3000;
        } else if( s < 0 ) {
            if( errno == EAGAIN || errno == EINPROGRESS ) {
                fd_set wfds;
                timeval tv = { 0, 10000 };
                FD_ZERO( &wfds );
                FD_SET( m_fd, &wfds );
                if( (rc = select( m_fd + 1, NULL, &wfds, NULL, &tv )) < 0 && errno != EINTR ) {
                    if( --wait_periods == 0 )
                        return -1;
                }
            } else
                return -1;
        }
    }
    return (int)rc;
}

int dm::TcpSocket::receive( sockaddr * from , uint8_t * buffer, size_t buffersz ) {
    timeval tv = { 0, 500000 };
    fd_set rfds;
    FD_ZERO( &rfds );
    FD_SET( m_fd, &rfds );

    if( select( m_fd + 1, &rfds, NULL, NULL, &tv ) <= 0 || !FD_ISSET( m_fd, &rfds ) )
        return -1;

   socklen_t socklen = sizeof( sockaddr_in );
   return recvfrom( m_fd, buffer, buffersz, 0, from, &socklen );
}

void dm::TcpSocket::f_open() {
    if( (m_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1 )
      throw std::logic_error( std::string( "sockerror: " ) + std::string( strerror( errno ) ) );

    long yes = 1;
    struct linger linger = { 0, 0 };
    setsockopt( m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) );
    setsockopt( m_fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof( int ) );
    yes = 20;
    setsockopt( m_fd, SOL_TCP,    TCP_KEEPCNT,  &yes, sizeof( int ) );
    yes = 180;
    setsockopt( m_fd, SOL_TCP,    TCP_KEEPIDLE, &yes, sizeof( int ) );
    yes = 60;
    setsockopt( m_fd, SOL_TCP,    TCP_KEEPINTVL,&yes, sizeof( int ) );
    setsockopt( m_fd, SOL_SOCKET, SO_LINGER, &linger, sizeof( linger ) );

    fcntl( m_fd, F_SETFL, fcntl( m_fd, F_GETFL, 0 ) | O_NONBLOCK );
    socklen_t errlen = sizeof (int);
    int err;
    if( (getsockopt( m_fd, SOL_SOCKET, SO_ERROR, &err, &errlen )) != 0 ) {
      ::close( m_fd );
      throw std::logic_error( std::string( "sockerror: " ) + std::string( strerror( err ) ) );
    }

    struct timeval tv = { 5, 0 };
    setsockopt( m_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof( tv ) );
    setsockopt( m_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof( tv ) );
}

void dm::TcpSocket::close() {
    ::close( m_fd );
}