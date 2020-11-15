#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace dm {
    class Base64 {
    public:
        Base64( const uint8_t * dec, size_t decsz );
        Base64( const std::string & enc );

        const std::string & enc() const {
            return m_enc;
        }
        const std::vector< uint8_t > & dec() const {
            return m_dec;
        }
        void append( std::vector< uint8_t > & to ) {
            to.insert( to.end(), m_dec.begin(), m_dec.end() );
        }

    private:
        std::string m_enc;
        std::vector< uint8_t > m_dec;
};

}  // namespace dm
