//
// Created by mkh on 25.11.2020.
//
#pragma once

#include <cstdint>
#include <vector>

namespace dm {
    namespace aac {
        class AuHeader {
        public:
            static const uint16_t kSize = 2;

            AuHeader() = default;
            AuHeader( uint8_t * data );

            uint16_t size() const {
                return m_size;
            }
            uint8_t index() const {
                m_index;
            }

        private:
            uint16_t m_size;
            uint8_t m_index;
        };

        class AuHeaderSection {
        public:
            AuHeaderSection( uint8_t * data );

            std::vector< AuHeader >::const_iterator begin() const {
                return m_headers.begin();
            }
            std::vector< AuHeader >::const_iterator end() const {
                return m_headers.end();
            }
            uint8_t * payload() {
                return m_payload;
            }

        private:
            uint8_t * m_payload;
            std::vector< AuHeader > m_headers;
        };
    }  // namespace aac
}  // namespace dm
