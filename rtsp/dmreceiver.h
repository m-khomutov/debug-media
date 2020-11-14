#pragma once
#include "core/dmbasereceiver.h"
#include "dmconnection.h"

#include <thread>
#include <atomic>

namespace dm {
    namespace rtsp {
        class Receiver : public BaseReceiver {
        public:
            Receiver( const char * source, const char* cert );
            ~Receiver();

            void updateStream() override;
            void askPosition() override;
            void setPosition( double pos ) override;
            void setParameter( const char * param ) override;
            void scale( float value ) override;
            void pause( float value ) override;
            void resume( float value ) override;

            //const Range range() const override; { return m_conn.range(); }
            void run() override;

        private:
            static const size_t RTPBUFSZ = 1<<16;

            std::thread m_thread;
            std::atomic_bool m_running{true};
            //basic::Viewer* m_viewer;
            Connection m_connection;

            std::vector< uint8_t > m_data;
        };
    }  // namespace rtsp
}  // namespace dm
