//
// Created by mkh on 10.12.2020.
//

#include "dmintra4x4.h"
#include "dmframe420.h"

#include <cstring>

dm::h264::intra4x4::LumaRefSamples::LumaRefSamples( const Frame420 & frame, int blk_x, int blk_y ) {
    memset( m_data, 0, sizeof(m_data) );
    if( blk_x > 3 ) {  // has another block to the left
        for( int i(0); i < 4; ++i )  // L K J I
            m_data[3 - i] = frame.lumaPixel( blk_x-1, blk_y+i );
    }
    if( blk_x > 3 && blk_y > 3 )  // hase other blocks to the left and on top
        m_data[4] = frame.lumaPixel( blk_x-1, blk_y-1 );  // M
    if( blk_y > 3 ) {  // has another block on top
        for( int i(0); i < 4; ++i )  // A B C D
            m_data[5 + i] = frame.lumaPixel( blk_x+i, blk_y-1 );

        if( blk_x <= frame.width()-8 ) {  // has another block to the right and on top
            for( int i(4); i < 8; ++i )  // E F G H
                m_data[5 + i] = frame.lumaPixel( blk_x+i, blk_y-1 );
        }
        else {  // if samples E, F, G and H are not available, the value of sample D is copied to these positions
            for( int i(4); i < 8; ++i)  // [E F G H] = D
                m_data[5 + i] = m_data[8];
        }
    }
}


void dm::h264::intra4x4::PredictionBlock::vertical( int blk_x, int blk_y, Frame420 * frame ) {
    intra4x4::LumaRefSamples ref( *frame, blk_x, blk_y );
    for( int y(0); y < 4; ++y )
        for( int x(0); x < 4; ++x )
            frame->lumaPixel( blk_x+x, blk_y+y ) = ref.top( x );
}
void dm::h264::intra4x4::PredictionBlock::horizontal( int blk_x, int blk_y, Frame420 * frame ) {
    intra4x4::LumaRefSamples ref( *frame, blk_x, blk_y );
    for( int y(0); y < 4; ++y )
        for( int x(0); x < 4; ++x )
            frame->lumaPixel( blk_x+x, blk_y+y ) = ref.left( y );
}
void dm::h264::intra4x4::PredictionBlock::diagonalDownLeft( int blk_x, int blk_y, Frame420 * frame ) {
    intra4x4::LumaRefSamples ref( *frame, blk_x, blk_y );
    for( int y(0); y < 4; ++y ) {
        for( int x(0); x < 4; ++x ) {
            if( (x & y)==3 ) {
                frame->lumaPixel( blk_x+x, blk_y+y ) = (ref.top( 6 ) + 3*ref.top( 7 ) + 2)>>2;
            }
            else
                frame->lumaPixel( blk_x+x, blk_y+y ) = (ref.top( x+y ) + 2*ref.top( x+y+1 ) + ref.top( x+y+2 ) + 2)>>2;
        }
    }
}
