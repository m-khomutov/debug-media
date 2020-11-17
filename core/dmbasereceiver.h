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
        std::string m_host;
        std::string m_path;
        std::string m_cert;
        std::string m_user;
        std::string m_password;
        in_addr_t m_ip;
        uint16_t  m_port{0};

    private:
        void f_parse_authenticated_url( const char * url );
        void f_parse_url( const char * url );
    };
}  // namespace dm
