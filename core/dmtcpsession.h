#pragma once
#include "dmbasesession.h"

#include <vector>
#include <string>

namespace dm {
    class TcpSession : public BaseSession {
    public:
        TcpSession( const char * source );
        ~TcpSession();

        void open() override;
        void close() override;

        void set( fd_set* rfds ) override;
        int fd() override;
        bool isset( fd_set* rfds ) override;

        int receive( uint8_t * buf, size_t bufsz, bool whole ) override;
        int send( const uint8_t * msg, size_t msgsz );

    private:
        TcpSocket m_socket;
        int m_connected {-1};
        sockaddr_in m_source;

    };
}  // namespace dm