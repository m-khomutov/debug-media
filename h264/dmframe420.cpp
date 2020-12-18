//
// Created by mkh on 10.12.2020.
//

#include "dmframe420.h"
#include "dmintra4x4.h"


dm::h264::Frame420::Frame420( int width, int height )
: m_luma_width( width ),
  m_luma_height( height ),
  m_chroma_width( width>>1 ),
  m_chroma_height( height>>1 ),
  m_luma( width*height ) {
    for( int i(0); i < 2; ++i )
        m_chroma[i].resize( (width*height)>>2 );
}
