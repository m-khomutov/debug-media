//
// Created by mkh on 23.11.2020.
//

#include "dmavichunk.h"

#include <iostream>
#include <stdexcept>
#include <cstring>

dm::avi::Chunk * dm::avi::Chunk::create( FILE * f ) {
    char name[5];
    if( fread( name, 1, sizeof(uint32_t), f ) != sizeof(uint32_t) )
        throw std::logic_error( "failed to read chunk name" );
    name[4] = 0;

    switch( str2type( name ) ) {
        case Type::kRIFF:
            return new RiffChunk( f );
        case Type::kAVI:
            return new AviChunk( f );
        case Type::kHdrl:
            return new HdrlChunk( f );
    }
    return nullptr;
}

const char * dm::avi::Chunk::type2str( Type type ) {
    switch( type ) {
        case Type::kRIFF:
            return "RIFF";
        case Type::kAVI:
            return "AVI ";
        case Type::kHdrl:
            return "hdrl";
    }
    return "";
}
dm::avi::Chunk::Type dm::avi::Chunk::str2type( const char * str ) {
    if( !strcmp( str, "RIFF" ) ) {
        return Type::kRIFF;
    }
    else if( !strcmp( str, "AVI " ) ) {
        return Type::kAVI;
    }
    else if( !strcmp( str, "hdrl" ) ) {
        return Type::kHdrl;
    }
    return Type::kUnknown;
}

dm::avi::RiffChunk::RiffChunk( FILE * f ) : Chunk( Type::kRIFF ){
    if( fread( &m_size, 1, sizeof(m_size), f ) != sizeof(m_size) )
        throw std::logic_error( "failed to read RIFF size" );
}

dm::avi::AviChunk::AviChunk( FILE * f ) : Chunk( Type::kAVI ){
    char buf[ 5 ] = {0,0,0,0,0};
    int len = fread( buf, 1, 4, f );
    if( len != 4 || strcmp( buf, "LIST" ) )
        throw std::logic_error( "failed to read AVI chunk" );
    if( fread( &m_size, 1, sizeof(m_size), f ) != sizeof(m_size) )
        throw std::logic_error( "failed to read AVI size" );
}

dm::avi::HdrlChunk::HdrlChunk( FILE * f ) : Chunk( Type::kHdrl ) {
    char buf[5] = {0,0,0,0,0};
    int len = fread( buf, 1, 4, f );
    if( len != 4 || strcmp( buf, "avih" ) )
        throw std::logic_error( "failed to get hdrl chunk" );
    if( (len = fread( &m_size, 1, sizeof(m_size), f )) != sizeof(m_size) )
        throw std::logic_error( "failed to get hdrl chunk size" );
    if( (len = fread( &m_header, 1, sizeof(m_header), f )) != sizeof(m_header) )
        throw std::logic_error( "failed to get hdrl chunk " );

    std::cerr << "mcs: " << m_header.microSecPerFrame << " " << m_header.width << "x" << m_header.height << std::endl;
}

dm::avi::File::File( const char *fname ) : m_fd( fopen( fname, "r" ), [](FILE*f){ fclose( f ); } ) {
    if( !m_fd )
        throw std::logic_error( strerror( errno ) );
    while( true ) {
        std::unique_ptr< Chunk> chunk( Chunk::create( m_fd.get() ) );
        if( chunk ) {
            std::cerr << "Type : " << Chunk::type2str( chunk->type() ) << " sz : " << chunk->size() << std::endl;
            continue;
        }
        break;
    }
}
