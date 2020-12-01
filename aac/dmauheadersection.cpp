//
// Created by mkh on 25.11.2020.
//

#include "dmauheadersection.h"

#include <cstring>
#include <endian.h>

const char * dm::aac::AuHeader::Fields::fmtp_tags[] {
        "streamType=",
        "profile-level-id=",
        "config=",
        "mode=",
        "objectType=",
        "constantSize=",
        "constantDuration=",
        "maxDisplacement=",
        "de-interleaveBufferSize=",
        "sizeLength=",
        "indexLength=",
        "indexDeltaLength=",
        "CTSDeltaLength=",
        "DTSDeltaLength=",
        "randomAccessIndication=",
        "streamStateIndication=",
        "auxiliaryDataSizeLength=",
        "profile="
};

dm::aac::AuHeader::Fields::Mode::Mode( const char * fmtp ) {
    if( strcasestr( fmtp, "generic" ) ) {
        m_value = kGeneric;
    }
    else if( strcasestr( fmtp, "CELP-cbr" ) ) {
        m_value = kCELP_cbr;
    }
    else if( strcasestr( fmtp, "CELP-vbr" ) ) {
        m_value = kCELP_vbr;
    }
    else if( strcasestr( fmtp, "AAC-lbr" ) ) {
        m_value = kAAC_lbr;
    }
    else if( strcasestr( fmtp, "AAC-hbr" ) ) {
        m_value = kAAC_hbr;
    }
}

dm::aac::AuHeader::Fields::Fields( const std::string &fmtp ) {
    size_t from = 0;
    for( size_t i(0); i < sizeof(fmtp_tags) / sizeof(fmtp_tags[0]); ++i ) {
        const char * par = strcasestr( fmtp.c_str(), fmtp_tags[i] );
        if( par ) {
            switch( i ) {
                case kStreamType:
                    streamtype = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kProfileLevelId:
                    profileLevelId = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kConfig:
                    config = strtol( par + strlen(fmtp_tags[i]), nullptr, 16 );
                    break;
                case kMode:
                    mode = Mode( par + strlen(fmtp_tags[i]) );
                    break;
                case kObjectType:
                    objectType = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kConstantSize:
                    constantSize = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kConstantDuration:
                    constantDuration = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kMaxDisplacement:
                    maxDisplacement = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kDeinterleaveBufferSize:
                    deinterleaveBufferSize = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kSizeLength:
                    sizeLength = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kIndexLength:
                    indexLength = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kIndexDeltaLength:
                    indexDeltaLength = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kCTSDeltaLength:
                    CTSDeltaLength = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kDTSDeltaLength:
                    DTSDeltaLength = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kRandomAccessIndication:
                    randomAccessIndication = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kStreamStateIndication:
                    streamStateIndication = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kAuxiliaryDataSize:
                    auxiliaryDataSize = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
                case kProfile:
                    profile = strtol( par + strlen(fmtp_tags[i]), nullptr, 10 );
                    break;
            }
        }
    }
}

dm::aac::AuHeader::AuHeader( uint8_t *data, const Fields & fields, bool first ) {
    BitReader br( data );
    m_size = br.bits( fields.sizeLength );
    m_index = first ? br.bits( fields.indexLength ) : br.bits( fields.indexDeltaLength );
}

dm::aac::AuHeaderSection::AuHeaderSection( uint8_t *data, const AuHeader::Fields & auheader_fields ) {
    uint16_t headersCountBits;
    memcpy( &headersCountBits, data, sizeof(headersCountBits) );
    headersCountBits = be16toh( headersCountBits );
    data += sizeof(headersCountBits);

    for( uint16_t bit = 0; bit < headersCountBits; ) {
        m_headers.emplace_back( data, auheader_fields, bit );
        uint16_t headerlen_bits = auheader_fields.constantSize;
        if( !headerlen_bits ) {
            headerlen_bits = auheader_fields.sizeLength;
            headerlen_bits += bit ? auheader_fields.indexLength : auheader_fields.indexDeltaLength;
            headerlen_bits += auheader_fields.CTSDeltaLength ? 1 + auheader_fields.CTSDeltaLength: 0;
            headerlen_bits += auheader_fields.DTSDeltaLength ? 1 + auheader_fields.DTSDeltaLength: 0;
            headerlen_bits += auheader_fields.randomAccessIndication ? 1 : 0;
            headerlen_bits += auheader_fields.streamStateIndication;
        }
        data += (headerlen_bits + auheader_fields.auxiliaryDataSize) >> 3;  // in bytes
        bit += headerlen_bits;
    }
    m_payload = data;
}