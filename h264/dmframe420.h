//
// Created by mkh on 10.12.2020.
//

#pragma once

#include <cstdint>
#include <vector>

namespace dm {
    namespace h264 {
        class Frame420 {
        public:
            Frame420( int width, int height );

            int width() const {
                return m_luma_width;
            }
            int height() const {
                return m_luma_height;
            }
            uint8_t lumaPixel( int x, int y ) const {
                return m_luma[y*m_luma_width + x];
            }
            uint8_t & lumaPixel( int x, int y ) {
                return m_luma[y*m_luma_width + x];
            }
            uint8_t chromaPixel( int iCbCr, int x, int y ) const {
                return m_chroma[iCbCr][y*m_chroma_width + x];
            }
            uint8_t & chromaPixel( int iCbCr, int x, int y ) {
                return m_chroma[iCbCr][y*m_chroma_width + x];
            }

        private:
            int m_luma_width;
            int m_luma_height;
            int m_chroma_width;
            int m_chroma_height;

            std::vector< uint8_t > m_luma;
            std::vector< uint8_t > m_chroma[2];
        };
    }  // namespace h264
}  // namespace dm