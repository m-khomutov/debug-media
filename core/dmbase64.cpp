#include "dmbase64.h"

#include <string.h>
#include <stdlib.h>

#include <memory>
#include <functional>
#include <openssl/bio.h>
#include <openssl/evp.h>

dm::Base64::Base64( const uint8_t * dec, size_t decsz ) {
    std::unique_ptr< BIO, std::function< void(BIO*) > > b64(BIO_new( BIO_f_base64() ), [](BIO *p){ BIO_free_all(p); });
    BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
    BIO* sink = BIO_new(BIO_s_mem());
    BIO_push(b64.get(), sink);
    BIO_write(b64.get(), dec, decsz );
    BIO_flush(b64.get());

    const char* enc;
    long len = BIO_get_mem_data( sink, &enc );
    m_enc.assign( enc, len );
}

dm::Base64::Base64( const std::string & enc ) {
    std::unique_ptr< BIO, std::function< void(BIO*) > > b64(BIO_new( BIO_f_base64() ), [](BIO *p){ BIO_free_all(p); });
    BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
    BIO* source = BIO_new_mem_buf( enc.c_str(), -1 ); // read-only source
    BIO_push( b64.get(), source );
    const int maxlen = enc.length() / 4 * 3 + 1;
    m_dec.resize( maxlen + 4 );
    const int len = BIO_read( b64.get(), m_dec.data(), maxlen );
    m_dec.resize( len );
}