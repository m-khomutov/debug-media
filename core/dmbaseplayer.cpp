//
// Created by mkh on 16.11.2020.
//

#include "dmbaseplayer.h"
#include "dmqplayer.h"

dm::BasePlayer * dm::BasePlayer::create( int width, int height ) {
    return new q::Player( width, height, 10 );
}
