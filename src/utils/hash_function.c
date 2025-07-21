#include "hash_function.h"

uint8_t
hash_8bit(const void* data, size_t len) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint8_t hash = 0x5A;           // Initial seed value
    const uint8_t multiplier = 31; // Small prime multiplier

    for (size_t i = 0; i < len; i++) {
        hash = (hash * multiplier) + bytes[i];
        // hash = b₀ * 31ⁿ⁻¹ + b₁ * 31ⁿ⁻² + ... + bₙ₋₁ * 31⁰
    }

    return hash;
}

uint16_t
hash_12bit(const void* data, size_t len) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint16_t hash = 0x9C4;       // 12-bit FNV offset basis approximation
    const uint16_t prime = 0x93; // Small prime for 12-bit space

    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i]; // XOR current byte into hash
        hash *= prime;    // Multiply to diffuse bits
        hash &= 0xFFF;    // Mask to keep only 12 bits (4096 values)
    }

    return hash;
}

uint16_t
hash_16bit(const void* data, size_t len) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint16_t hash = 0xFFFF;             // Initialize with all bits set
    const uint16_t polynomial = 0x8408; // Reversed CRC-16 polynomial

    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i]; // XOR byte into lower bits

        for (int bit = 0; bit < 8; bit++) {
            if (hash & 1) {
                hash = (hash >> 1) ^ polynomial;
            } else {
                hash >>= 1;
            }
        }
    }

    return hash;
}

unsigned char*
openssl_hash(const void* data, size_t len, enum openssl_hash_function_ids hash_id) {
    // 0. Set the correct hash function based on the ID
    const EVP_MD* (*hash_fn)(void) = NULL;

    switch (hash_id) {
        case BH_OPENSSL_HASH_RIPEMD160: hash_fn = EVP_ripemd160; break;
        case BH_OPENSSL_HASH_SHA1: hash_fn = EVP_sha1; break;
        case BH_OPENSSL_HASH_SHA3_256: hash_fn = EVP_sha3_256; break;
        case BH_OPENSSL_HASH_SHA256: hash_fn = EVP_sha256; break;
        case BH_OPENSSL_HASH_SHA512: hash_fn = EVP_sha512; break;
        case BH_OPENSSL_HASH_SHA384: hash_fn = EVP_sha384; break;
        default: return NULL; // Unsupported hash function ID
    }

    // 1. Buffer to store the hash
    const EVP_MD* md = hash_fn();
    if (!md) {
        return NULL;
    }

    unsigned char* hash = malloc(EVP_MD_size(md));
    unsigned int hash_len = 0;

    // 2. Create and initialize the message digest context
    EVP_MD_CTX* ctx = EVP_MD_CTX_new(); // dynamically allocate context
    if (ctx == NULL) {
        free(hash);  // Free the hash buffer on error
        return NULL; // Return NULL if context creation fails
    }

    // 3. Initialize the context with hash algorithm
    if (EVP_DigestInit_ex(ctx, md, NULL) != 1) {
        free(hash);           // Free the hash buffer on error
        EVP_MD_CTX_free(ctx); // Free the context on error
        return NULL;
    }

    // 4. Feed data into the hash function
    if (EVP_DigestUpdate(ctx, data, len) != 1) {
        free(hash);           // Free the hash buffer on error
        EVP_MD_CTX_free(ctx); // Free the context on error
        return NULL;
    }

    // 5. Finalize and get the digest output
    if (EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) {
        free(hash);           // Free the hash buffer on error
        EVP_MD_CTX_free(ctx); // Free the context on error
        return NULL;
    }

    // 6. Free the context
    EVP_MD_CTX_free(ctx);

    return hash;
}