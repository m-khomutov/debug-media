//
// Created by mkh on 16.11.2020.
//

#include "dmbitreader.h"

dm::h264::BitReader::BitReader( const uint8_t *data, size_t datasz ) {
    size_t i{0};
    while( i < datasz ) {
        if( i+2 < datasz && data[i]==0 && data[i+1]==0 && data[i+2]==3 ) { // emulation_prevention_three_byte equal to 0 0 3
            m_rbsp.push_back( data[i] );
            m_rbsp.push_back( data[i+1] );
            i += 3;
        }
        else {
            m_rbsp.push_back( data[i] );
            ++i;
        }
    }
}

uint32_t dm::h264::BitReader::bit() {
    return f_get_bit();
}

uint32_t dm::h264::BitReader::bits( uint32_t bitnum ) {
    if( bitnum == 0 )
        throw EndOfRBSP();
    if( bitnum == 1 )
        return f_get_bit();

    uint32_t ret{0};
    uint32_t i{0};
    while( i < bitnum ) {
        ret |= f_get_bit() << (bitnum-(i+1));
        ++i;
    }
    return ret;
}

uint32_t dm::h264::BitReader::byte() {
    return bits( 8 );
}
uint32_t dm::h264::BitReader::word() {
    return bits( 16 );
}
uint32_t dm::h264::BitReader::dword() {
    return bits( 32 );
}
uint32_t dm::h264::BitReader::uGolomb() {
    uint32_t zeroes{0};
    while( f_get_bit() == 0 )
        ++zeroes;

    if( m_offset + zeroes < m_rbsp.size() * 8 ) {
        uint32_t ret = uint32_t(1) << zeroes;
        for( int32_t i(zeroes - 1); i >= 0; --i )
            ret |= f_get_bit() << i;
        return ret - 1;
    }
    throw EndOfRBSP();
}
int32_t dm::h264::BitReader::sGolomb() {
    uint32_t ret = uGolomb();

    size_t rem = ret % 2;
    if( !rem )
        return -1 * (ret >> 1);

    return (ret >> 1) + 1;
}

uint32_t dm::h264::BitReader::f_get_bit() const {
    if( m_offset < m_rbsp.size() * 8 ) {
        uint64_t off = m_offset++;
        return ((m_rbsp.data()[off >> 3]) >> (7 - (off % 8))) & 1;
    }
    throw EndOfRBSP();
}