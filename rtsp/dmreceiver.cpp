#include "dmreceiver.h"

#include <iostream>

dm::rtsp::Receiver::Receiver( const char * source, const char * cert )
: BaseReceiver( source, cert ),
  m_connection( m_url, m_path, m_user, m_password ),
  m_data( 0xffff ) {
    m_interleaved_buffer.data = m_data.data();
}

dm::rtsp::Receiver::~Receiver() {
    if( m_thread.joinable() ) {
        m_running.store( false );
        m_thread.join();
    }
}

void dm::rtsp::Receiver::run () {
    try {
        m_connection.open();

        m_thread = std::thread( [this](){
            while( m_running ) {
                fd_set rfds;
                FD_ZERO( &rfds );
                m_connection.set( &rfds );

                timeval tv{ 0, 100 };
                if( (select( m_connection.fd() + 1, &rfds, NULL, NULL, &tv )) > 0 )
                    m_connection.receive( &rfds, &m_interleaved_buffer );
                std::this_thread::yield();
            }
        });
    }
    catch( const std::logic_error& err ) {
        std::cerr << err.what();
    }
}