//
// Created by mkh on 25.11.2020.
//
#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace dm {
    namespace aac {
        class AuHeader {
        public:
            struct Fields {
                enum FmtpTags {
                    kStreamType,
                    kProfileLevelId,
                    kConfig,
                    kMode,
                    kObjectType,
                    kConstantSize,
                    kConstantDuration,
                    kMaxDisplacement,
                    kDeinterleaveBufferSize,
                    kSizeLength,
                    kIndexLength,
                    kIndexDeltaLength,
                    kCTSDeltaLength,
                    kDTSDeltaLength,
                    kRandomAccessIndication,
                    kStreamStateIndication,
                    kAuxiliaryDataSize,
                    kProfile
                };
                static const char * fmtp_tags[];
                class Mode {
                public:
                    enum Value { kGeneric, kCELP_cbr, kCELP_vbr, kAAC_lbr, kAAC_hbr };
                    Mode() = default;
                    Mode( const char * fmtp );

                    operator Value() const {
                        return m_value;
                    }

                private:
                    Value m_value {kGeneric};
                };

                Fields( const std::string & fmtp );

                int streamtype{0};
                int profileLevelId{0};
                int config{0};
                Mode mode;
                int objectType{0};
                uint32_t constantSize{0};
                uint32_t constantDuration{0};
                uint32_t maxDisplacement{0};
                uint32_t deinterleaveBufferSize{0};
                int sizeLength{0};
                int indexLength{0};
                int indexDeltaLength{0};
                int CTSDeltaLength{0};
                int DTSDeltaLength{0};
                int randomAccessIndication{0};
                int streamStateIndication{0};
                int auxiliaryDataSize{0};
                int profile{0};

                uint32_t hedarSize{0};
            };
            class BitReader {
            public:
                BitReader( uint8_t * data ) : m_data( data ) {}

                bool bit() {
                    uint64_t off = m_off++;
                    return ((m_data[off >> 3]) >> (7 - (off % 8))) & 1;
                }
                uint32_t bits( uint32_t bitcount ) {
                    if( bitcount == 1 )
                        return bit();

                    uint32_t ret{0};
                    uint32_t i{1};
                    while( i <= bitcount ) {
                        ret |= bit() << (bitcount-i);
                        ++i;
                    }
                    return ret;
                }

            private:
                uint8_t * m_data;
                uint64_t m_off{0};
            };

            AuHeader() = default;
            AuHeader( uint8_t * data, const Fields & fields, bool first );

            uint16_t size() const {
                return m_size;
            }
            uint8_t index() const {
                m_index;
            }

        private:
            uint16_t m_size;
            uint8_t m_index;
        };

        class AuHeaderSection {
        public:
            AuHeaderSection( uint8_t * data, const AuHeader::Fields & fields );

            std::vector< AuHeader >::const_iterator begin() const {
                return m_headers.begin();
            }
            std::vector< AuHeader >::const_iterator end() const {
                return m_headers.end();
            }
            uint8_t * payload() {
                return m_payload;
            }

        private:
            uint8_t * m_payload;
            std::vector< AuHeader > m_headers;
        };
    }  // namespace aac
}  // namespace dm
