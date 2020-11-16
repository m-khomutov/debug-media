//
// Created by mkh on 16.11.2020.
//
#pragma once

#include <stdint.h>
#include <vector>

namespace dm {
    namespace h264 {
        class BitReader {
        public:
            struct EndOfRBSP {};

            BitReader( const uint8_t * data, size_t datasz );

            uint32_t bit();
            uint32_t bits( uint32_t bitnum );
            uint32_t byte();
            uint32_t word();
            uint32_t dword();
            uint32_t uGolomb();
            int32_t sGolomb();

        private:
            std::vector< uint8_t > m_rbsp;
            uint64_t m_offset{0};

        private:
            uint32_t f_get_bit_by_offset( uint64_t offset ) const;
        };

    } // namespace h264
}  // namespace dm
