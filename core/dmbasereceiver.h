#pragma once

#include "dmbaseplayer.h"
#include <netdb.h>

#include <string>
#include <stdexcept>

namespace dm {
    class BaseReceiver {
    public:
        static BaseReceiver* create( const char * source, const char* cert );

        BaseReceiver( const char * source, const char* cert );
        virtual ~BaseReceiver() = default;

        virtual void run() {}

    protected:
        std::string m_url;
        std::string m_path;
        std::string m_cert;
        in_addr_t m_ip;
        uint16_t  m_port;
    };
}  // namespace dm
