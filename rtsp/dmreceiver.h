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

            void run() override;

        private:
            std::thread m_thread;
            std::atomic_bool m_running{true};
            Connection m_connection;

            std::vector< uint8_t > m_data;
            Connection::InterleavedBuffer m_interleaved_buffer;
        };
    }  // namespace rtsp
}  // namespace dm
