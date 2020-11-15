#include "dmreceiver.h"
//#include "../basic/seagull_viewer.h"

#include <iostream>

dm::rtsp::Receiver::Receiver( const char * source, const char * cert )
: BaseReceiver( source, cert ),
    //m_viewer(viewer),
  m_connection( m_url.c_str(), m_path.c_str() ),
  m_data( 0xffff ) {
    m_interleaved_buffer.data = m_data.data();
   //m_viewer->updateCachedDuration( basic::Receiver::CACHE_SIZE_MSEC + 5. );
}

dm::rtsp::Receiver::~Receiver() {
    if( m_thread.joinable() ) {
        m_running.store( false );
        m_thread.join();
    }
}

void dm::rtsp::Receiver::updateStream () {
////   m_conn.ping();

//   m_viewer->updateCachedDuration( basic::Receiver::CACHE_SIZE_MSEC + 5. );
}

void dm::rtsp::Receiver::askPosition () {
   m_connection.getParameter( std::string( "position" ) );
}

void dm::rtsp::Receiver::setPosition( double pos ) {
   m_connection.setPosition( (int)pos );
}

void dm::rtsp::Receiver::setParameter( const char* param ) {
   m_connection.setParameter( param );
}

void dm::rtsp::Receiver::scale( float value ) {
   m_connection.scale( value );
}

void dm::rtsp::Receiver::pause( float value ) {
   m_connection.pause( value );
}

void dm::rtsp::Receiver::resume( float position ) {
   m_connection.resume( position );
}


void dm::rtsp::Receiver::run () {
    try {
        m_connection.open( /*viewer()*/ );

        m_thread = std::thread( [this](){
            while( m_running ) {
                fd_set rfds;
                FD_ZERO( &rfds );
                m_connection.set( &rfds );

                timeval tv{ 0, 100 };
                if( (select( m_connection.fd() + 1, &rfds, NULL, NULL, &tv )) > 0 ){
                    int len = m_connection.receive( &rfds, &m_interleaved_buffer );
                    std::cerr << "recvd  chan: " << int(m_interleaved_buffer.channel) << " : sz=" << m_interleaved_buffer.size << std::endl;
                }

                std::this_thread::yield();
            }
        });
    }
    catch( const std::logic_error& err ) {
        std::cerr << err.what();
    }
}