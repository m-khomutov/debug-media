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
