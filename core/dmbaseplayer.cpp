//
// Created by mkh on 16.11.2020.
//

#include "dmbaseplayer.h"
#include "dmqplayer.h"

dm::BasePlayer * dm::BasePlayer::create( int width, int height, rtsp::MediaSession * session ) {
    return new q::Player( session, width, height, 10 );
}
