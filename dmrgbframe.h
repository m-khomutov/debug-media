#pragma once

#include <array>
#include <utility>

extern "C" {
#include <libswscale/swscale.h>
}

namespace dm {
    namespace rgb {
        struct Geometry {
            uint32_t width;
            uint32_t height;
            AVPixelFormat format;

            Geometry( uint32_t w =0, uint32_t h =0, AVPixelFormat f =AV_PIX_FMT_YUV420P ) : width( w ),height( h ),format( f ) {}
        };
        class Frame {
        public:
            Frame(const Geometry& =Geometry());
            ~Frame();

            void copy(const uint8_t**, int*, uint32_t, uint32_t);
            void setGeometry( const Geometry & g ) {
                m_output_geometry = g;
            }

            uint32_t width() const {
                return m_output_geometry.width;
            }
            uint32_t height() const {
                return m_output_geometry.height;
            }
            uint8_t* data() const {
                return m_planes[ 0 ];
            }

        private:
            SwsContext * m_swsCtx{nullptr};

            Geometry m_input_geometry;
            Geometry m_output_geometry;
            uint32_t m_rescale_flags;
            uint8_t * m_planes[ 4 ]{nullptr, nullptr, nullptr, nullptr};
            int m_linesize[ 4 ];

        private:
            void open( int width, int height );
            void close();
        };
    }  // namespace rgb
}  // namespace dm
