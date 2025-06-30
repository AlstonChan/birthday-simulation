#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <stdint.h>

/**
 * @brief 8-bit hash function using simple polynomial rolling hash
 *
 * This function implements a basic polynomial rolling hash with a small
 * multiplier to create an 8-bit hash output. Due to the small output space
 * (256 possible values), collisions are expected frequently.
 *
 * @param data Pointer to input data buffer
 * @param len Length of input data in bytes, exp: 1-255
 * @return uint8_t 8-bit hash value (0-255)
 */
uint8_t hash_8bit(const void *data, size_t len);

/**
 * @brief 12-bit hash function using modified FNV-like algorithm
 *
 * This function implements a modified FNV (Fowler-Noll-Vo) hash algorithm
 * truncated to 12 bits. The output space is 4096 possible values, making
 * it suitable for demonstrating birthday paradox with moderate collision rates.
 *
 * @param data Pointer to input data buffer
 * @param len Length of input data in bytes
 * @return uint16_t 12-bit hash value (0-4095), stored in lower 12 bits
 */
uint16_t hash_12bit(const void *data, size_t len);

/**
 * @brief 16-bit hash function using CRC-like polynomial
 *
 * This function implements a simplified CRC-like hash using polynomial
 * arithmetic. The 16-bit output space (65536 values) provides a good
 * balance for birthday attack demonstration.
 *
 * @param data Pointer to input data buffer
 * @param len Length of input data in bytes
 * @return uint16_t 16-bit hash value (0-65535)
 */
uint16_t hash_16bit(const void *data, size_t len);

enum openssl_hash_function_ids {
    BH_OPENSSL_HASH_RIPEMD160,
    BH_OPENSSL_HASH_SHA1,
    BH_OPENSSL_HASH_SHA3_256,
    BH_OPENSSL_HASH_SHA256,
    BH_OPENSSL_HASH_SHA512,
    BH_OPENSSL_HASH_SHA384,
};

/**
 * @brief Generic wrapper for OpenSSL hash functions
 * 
 * REMEMBER TO `free()` the returned pointer after use.
 *
 *
 * @param data Pointer to input data buffer
 * @param len Length of input data in bytes
 * @param hash_id The ID of the hash function to use
 * @return unsigned char* Pointer to the 20-byte hash value, caller must free it
 */
unsigned char *openssl_hash(const void *data, size_t len,enum openssl_hash_function_ids hash_id);



#endif 