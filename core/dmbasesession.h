#pragma once
#include "dmtcpsocket.h"

#include <vector>
#include <string>

namespace dm {
    class BaseSession {
    public:
        static const int OK = 200;

        static BaseSession* create( const char * source, const char * pemkey =0 );

        BaseSession( const char * source );
        virtual ~BaseSession() = default;

        virtual void open () =0;
        virtual void close() =0;

        virtual int receive( uint8_t *buf, size_t bufsz, bool whole )  =0;
        virtual int send( const uint8_t * msg, size_t msgsz )  =0;

        virtual void set( fd_set* rfds ) {}
        virtual int  fd() { return -1; }
        virtual bool isset( fd_set* rfds ) { return false; }

        int getChar( char * c );
        int putLine( const char* line );
        int putRequest( const char* protoline, const std::vector< std::string > & headers, const char * body =0 );

        const std::string& source () const {
            return m_source;
        }
        sockaddr* srcAddress() {
            return (sockaddr*)&m_src_address;
        }

    private:
        static const char kEol[ 2 ];

        std::string m_source;
        sockaddr_in m_src_address;
    };
}  // namespace dm