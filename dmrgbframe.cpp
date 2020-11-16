#include "dmrgbframe.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/mem.h>
}

#include <algorithm>

dm::rgb::Frame::Frame( const Geometry & g ): m_output_geometry( g ),m_rescale_flags( SWS_BILINEAR ) {}

dm::rgb::Frame::~Frame() {
  close();
}

void dm::rgb::Frame::close() {
    if( m_planes[ 0 ] ) {
        av_freep(&m_planes[ 0 ]);
        sws_freeContext( m_swsCtx );
        delete [] m_planes[ 0 ];
    }
}

void dm::rgb::Frame::open( int w, int h ) {
    if( !m_planes[ 0 ]) {
        m_input_geometry.width  = w;
        m_input_geometry.height = h;
        m_swsCtx = sws_getContext( m_input_geometry.width, m_input_geometry.height,  m_input_geometry.format,
                                   m_output_geometry.width, m_output_geometry.height, m_output_geometry.format,
                                   m_rescale_flags, 0, 0, 0 );
        av_image_alloc( m_planes, m_linesize, m_output_geometry.width, m_output_geometry.height, m_output_geometry.format, 1 );
    }
}

void dm::rgb::Frame::copy( const uint8_t** data, int* linesize, uint32_t w, uint32_t h ) {
    if( w != m_input_geometry.width || h != m_input_geometry.height ) {
        close();
        open( w, h );
    }
    sws_scale( m_swsCtx, data, linesize, 0, h, m_planes, m_linesize );
}
