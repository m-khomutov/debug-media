//
// Created by mkh on 25.11.2020.
//

#include "dmauheadersection.h"

#include <cstring>
#include <endian.h>

dm::aac::AuHeader::AuHeader( uint8_t *data ) {
    uint16_t value;
    memcpy( &value, data, sizeof(value) );
    value = be16toh( value );

    m_size  = value >> 3;
    m_index = value & 0x07;
}

dm::aac::AuHeaderSection::AuHeaderSection( uint8_t *data ) {
    uint16_t headersCount;
    memcpy( &headersCount, data, sizeof(headersCount) );
    headersCount = be16toh( headersCount ) / (8 * AuHeader::kSize);
    data += sizeof(headersCount);

    for( uint16_t i = 0; i < headersCount; i++) {
        m_headers.push_back( data );
        data += AuHeader::kSize;
    }
    m_payload = data;
}