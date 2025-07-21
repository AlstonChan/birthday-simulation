#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <openssl/evp.h>
#include <stdint.h>

enum openssl_hash_function_ids {
    BH_OPENSSL_HASH_RIPEMD160,
    BH_OPENSSL_HASH_SHA1,
    BH_OPENSSL_HASH_SHA3_256,
    BH_OPENSSL_HASH_SHA256,
    BH_OPENSSL_HASH_SHA512,
    BH_OPENSSL_HASH_SHA384,
};

uint8_t hash_8bit(const void* data, size_t len);

uint16_t hash_12bit(const void* data, size_t len);

uint16_t hash_16bit(const void* data, size_t len);

unsigned char* openssl_hash(const void* data, size_t len, enum openssl_hash_function_ids hash_id);

#endif