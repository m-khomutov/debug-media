#pragma once

#include "dmbaseplayer.h"
#include <netdb.h>

#include <string>
#include <stdexcept>

namespace dm {
    using Range = std::pair< double, double >;

    class BaseReceiver {
    public:
        static BaseReceiver* create( const char * source, const char* cert );

        BaseReceiver( const char * source, const char* cert );
        virtual ~BaseReceiver() = default;

        double duration ();

        virtual void updateStream() =0;

        virtual void run() {}
        virtual void askPosition() {}
        virtual void setPosition( double pos ) {}
        virtual void setParameter( const char * param ) {}
        virtual void scale( float value ) {}
        virtual void pause( float value ) {}
        virtual void resume( float value ) {}

        virtual Range range() const { return Range(); }

    protected:
        std::string m_url;
        std::string m_path;
        std::string m_cert;
        in_addr_t m_ip;
        uint16_t  m_port;
    };
}  // namespace dm
