#pragma once
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdexcept>

namespace dm {
    class TcpSocket {
    public:
        TcpSocket();
        ~TcpSocket();

        operator int();

        void set( fd_set* rfds );
        bool isset( fd_set* rfds );

        int connect( sockaddr* to );
        int send( const uint8_t*,size_t );
        int receive( sockaddr*, uint8_t*,size_t );
        void reopen();

    private:
        int m_fd;

    private:
        void f_open();
    };
}  // namespace dm