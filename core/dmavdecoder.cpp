//
// Created by mkh on 25.11.2020.
//

#include "dmavdecoder.h"

#include <mutex>

namespace {
    int setLock( void** m, AVLockOp op ) {
        std::mutex*& mutex = *(std::mutex**)m;
        switch( op ) {
            case AV_LOCK_CREATE:
                mutex = new std::mutex();
                break;
            case AV_LOCK_DESTROY:
                delete mutex;
                break;
            case AV_LOCK_OBTAIN:
                mutex->lock();
                break;
            case AV_LOCK_RELEASE:
                mutex->unlock();
                break;
        }
        return 0;
    }
}  // namespace

dm::av::Decoder::Decoder( AVCodecID codecId ) {
    av_register_all();
    avcodec_register_all();
    av_lockmgr_register( setLock );

    if( !(m_codec = avcodec_find_decoder( codecId )) )
        throw std::logic_error( "avcodec_find_decoder failed" );

    m_codecCtx.reset( avcodec_alloc_context3( m_codec ) );
    if( !m_codecCtx.get() )
        throw std::logic_error( "avcodec_alloc_context3 failed" );

    m_codecCtx->codec = m_codec;
    m_codecCtx->extradata = nullptr;
    m_codecCtx->extradata_size = 0;
    m_codecCtx->thread_count = 1;//std::thread::hardware_concurrency();

    m_avFrame.reset( av_frame_alloc() );
    if( !m_avFrame.get() )
        throw std::logic_error( "av_frame_alloc failed" );
}

