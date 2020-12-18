//
// Created by mkh on 10.12.2020.
//

#pragma once

#include <stdint.h>

namespace dm {
    namespace h264 {

        class Frame420;

        namespace intra4x4 {
            /* M A B C D E F G H
             * I a b c d
             * J e f g h
             * K i j k l
             * L m n o p
            */
            class LumaRefSamples {
            public:
                LumaRefSamples( const Frame420 & frame, int blk_x, int blk_y );

                int left( int y ) const {
                    return m_data[3 - y]; // L K J I
                }
                int top( int x ) const {
                    return m_data[5 + x];  // A B C D
                }
                int M() const {
                    return m_data[4];
                }

            private:
                uint8_t m_data['N'-'A']; // L K J I M A B C D E F G H
            };

            class PredictionBlock {
            public:
                void vertical( int blk_x, int blk_y, Frame420 * frame );
                void horizontal( int blk_x, int blk_y, Frame420 * frame );
                void diagonalDownLeft( int blk_x, int blk_y, Frame420 * frame );
            };
        }  // namespace intra4x4
    }  // namespace h264
}  // namespace dm
