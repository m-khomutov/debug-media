//
// Created by mkh on 23.11.2020.
//
#pragma once

#include <cstdio>
#include <cstdint>
#include <memory>
#include <functional>

namespace dm {
    namespace avi {
        enum FourCC {
            kRiff = 0x46464952,
            kAvi  = 0x20495641,
            kList = 0x5453494c
        };
        class RiffHeader {
        public:
            static const uint32_t kSize = 3 * sizeof(uint32_t);

            RiffHeader( FourCC hdrType, const uint8_t * data ) {
                FourCC t = *(FourCC*)data;
                if( t != hdrType )
                    throw std::logic_error( "bad RIFF format" );

                m_size = be32toh( *(uint32_t*)data + sizeof(uint32_t) );
                m_type = FourCC(*(uint32_t*)(data + 2*sizeof(uint32_t)));
            }
            RiffHeader( FourCC hdrType, FILE * f ) {
                FourCC t;
                if( fread( &t, 1, sizeof(uint32_t), f ) != sizeof(uint32_t) || t != hdrType )
                    throw std::logic_error( "bad RIFF format" );

                if( fread( &m_size, 1, sizeof(uint32_t), f ) != sizeof(uint32_t) ||
                    fread( &m_type, 1, sizeof(uint32_t), f ) != sizeof(uint32_t) )
                    throw std::logic_error( "failed to read RIFF format" );
            }

            uint32_t size() const {
                return m_size;
            }
            FourCC type() const {
                return m_type;
            }

        private:
            uint32_t m_size;
            FourCC m_type;
        };
        class ChunkHeader {
        public:
            static const uint32_t kSize = 2 * sizeof(uint32_t);

            ChunkHeader( const uint8_t * data ) {
                m_id = FourCC(*(uint32_t*)(data + 2*sizeof(uint32_t)));
                m_size = be32toh( *(uint32_t*)data + sizeof(uint32_t) );
            }

            uint32_t size() const {
                return m_size;
            }
            FourCC id() const {
                return m_id;
            }

        private:
            FourCC m_id;
            uint32_t m_size;
        };

        class Chunk {
        public:
            enum class Type {
                kUnknown,
                kRIFF,
                kAVI,
                kHdrl
            };
            static Chunk * create( FILE * f );
            static const char * type2str( Type type );
            static Type str2type( const char * str );

            virtual ~Chunk() = default;

            uint32_t size() const {
                return m_size;
            }
            Type type() const {
                return m_type;
            }

        protected:
            Chunk( Type t ) : m_type( t ) {}

        protected:
            uint32_t m_size;

        private:
            Type m_type{Type::kUnknown};
        };

        class RiffChunk : public Chunk {
        public:
            RiffChunk( FILE * f );
        };

        class AviChunk : public Chunk {
        public:
            AviChunk( FILE * f );
        };

        class HdrlChunk : public Chunk {
        public:
            struct MainHeader {
                uint32_t microSecPerFrame;
                uint32_t maxBytesPerSec;
                uint32_t reserved1;
                uint32_t flags;
                uint32_t totalFrames;
                uint32_t initialFrames;
                uint32_t streams;
                uint32_t suggestedBufferSize;
                uint32_t width;
                uint32_t height;
                uint32_t scale;
                uint32_t rate;
                uint32_t start;
                uint32_t length;
            };

            HdrlChunk( FILE * f );

        private:
            MainHeader m_header;
        };
        class File {
        public:
            File( const char * fname );

        private:
            std::unique_ptr< FILE, std::function< void(FILE*) > > m_fd;
        };
    }  // namespace avi
}  // namespace dm
