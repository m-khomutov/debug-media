//
// Created by mkh on 15.11.2020.
//

#include "dmmediasession.h"

void dm::rtsp::MediaSession::setTransport( const char* line ) {
    const char* ptr = strstr( line, "interleaved=" );
    if( ptr ) {
        m_interleaved_port[0] = ptr[12] - '0';
        m_interleaved_port[1] = ptr[14] - '0';
    }
}

void dm::rtsp::MediaSession::setId( const char* line ) {
    const char* ptr = strchr( line, ';' );
    m_id = ptr ? std::string( line, ptr - line ) : std::string( line );

    if ((ptr = strstr(line, "timeout=")))
        m_timeout = strtol( ptr + 8, 0, 10 );
}
