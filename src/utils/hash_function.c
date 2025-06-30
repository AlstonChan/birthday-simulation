#include <openssl/evp.h>
#include <stdint.h>

#include "hash_function.h"

uint8_t hash_8bit(const void *data, size_t len) {
  const uint8_t *bytes = (const uint8_t *)data;
  uint8_t hash = 0x5A;           // Initial seed value
  const uint8_t multiplier = 31; // Small prime multiplier

  for (size_t i = 0; i < len; i++) {
    hash = (hash * multiplier) + bytes[i];
    // hash = b₀ * 31ⁿ⁻¹ + b₁ * 31ⁿ⁻² + ... + bₙ₋₁ * 31⁰
  }

  return hash;
}

uint16_t hash_12bit(const void *data, size_t len) {
  const uint8_t *bytes = (const uint8_t *)data;
  uint16_t hash = 0x9C4;       // 12-bit FNV offset basis approximation
  const uint16_t prime = 0x93; // Small prime for 12-bit space

  for (size_t i = 0; i < len; i++) {
    hash ^= bytes[i]; // XOR current byte into hash
    hash *= prime;    // Multiply to diffuse bits
    hash &= 0xFFF;    // Mask to keep only 12 bits (4096 values)
  }

  return hash;
}

uint16_t hash_16bit(const void *data, size_t len) {
  const uint8_t *bytes = (const uint8_t *)data;
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

unsigned char *hash_ripemd160(const void *data, size_t len) {
  // 1. Buffer to store the hash (RIPEMD-160 = 20 bytes)
  unsigned char *hash = malloc(EVP_MD_size(EVP_ripemd160()));
  unsigned int hash_len = 0;

  // 2. Create and initialize the message digest context
  EVP_MD_CTX *ctx = EVP_MD_CTX_new(); // dynamically allocate context
  if (ctx == NULL) {
    free(hash);  // Free the hash buffer on error
    return NULL; // Return NULL if context creation fails
  }

  // 3. Initialize the context with RIPEMD160 algorithm
  if (EVP_DigestInit_ex(ctx, EVP_ripemd160(), NULL) != 1) {
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