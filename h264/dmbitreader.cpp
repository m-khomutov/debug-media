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
    if( ++m_offset > m_rbsp.size() * 8 )
        throw EndOfRBSP();
    return f_get_bit_by_offset( m_offset-1 );
}

uint32_t dm::h264::BitReader::bits( uint32_t bitnum ) {
    if( bitnum == 0 )
        throw EndOfRBSP();
    if( bitnum == 1 )
        return bit();

    uint32_t ret{0};
    uint32_t i{0};
    while( i < bitnum ) {
        ret |= bit() << (bitnum-(i+1));
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
    while( bit() == 0 ) {
        if( m_offset >= m_rbsp.size() * 8 )
            throw EndOfRBSP();
        ++zeroes;
    }
    if( m_offset + zeroes > m_rbsp.size() * 8 )
        throw EndOfRBSP();

    uint32_t ret{uint32_t(1) << zeroes};
    for( int32_t i(zeroes - 1); i >= 0; --i )
        ret |= bit() << i;
    return ret - 1;
}
int32_t dm::h264::BitReader::sGolomb() {
    uint32_t ret{ uGolomb() };

    size_t rem = ret % 2;
    if( !rem )
        return -1 * (ret >> 1);

    return (ret >> 1) + 1;
}

uint32_t dm::h264::BitReader::f_get_bit_by_offset( uint64_t off ) const {
    return ((m_rbsp.data()[off >> 3]) >> (7 - (off % 8))) & 1;
}