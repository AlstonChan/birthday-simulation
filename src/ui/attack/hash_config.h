#ifndef HASH_CONFIG_H
#define HASH_CONFIG_H

#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "../menu.h"

enum hash_function_ids {
    HASH_CONFIG_8BIT,
    HASH_CONFIG_12BIT,
    HASH_CONFIG_16BIT,
    HASH_CONFIG_RIPEMD160,
    HASH_CONFIG_SHA1,
    HASH_CONFIG_SHA3_256,
    HASH_CONFIG_SHA256,
    HASH_CONFIG_SHA512,
    HASH_CONFIG_SHA384,
    HASH_CONFIG_KECCAK256
};

typedef struct {
    enum hash_function_ids id;         ///< The ID of the hash function
    const char *label;          ///< The label for the hash function
    const unsigned short bits;           ///< The bit size of the hash function
    const char *estimated_collisions; ///< Estimated collisions for the hash function
    const char *space_size;     ///< Space size of the hash function
} hash_config_t;

extern const hash_config_t hash_config[]; ///< Array of hash configurations
extern const unsigned short hash_config_len; ///< The number of hash configurations

/**
 * @brief Get the hash config menu object
 * 
 * @return struct ListMenuItem* 
 */
struct ListMenuItem *get_hash_config_menu();

/**
 * @brief Get the hash config item for a specific hash function ID
 * 
 * @param id The ID of the hash function
 * @return struct ListMenuItem The menu item for the specified hash function
 */
hash_config_t get_hash_config_item(enum hash_function_ids id);

/**
 * @brief Get maximum hash hex string length for a given hash function
 * 
 * It returns a uint16_t, because the maximum length of a hash function's hex string 
 * length in bits is definitely small enough to fit within a 16-bit unsigned integer.
 * Even like sha-512, has a 512-bit output, which is less than 2^16 - 1 = 65,535.
 * 
 * @param hash_id The ID of the hash function to get the hex length for
 * @return size_t The maximum length of the hash hex string, including the null terminator
 */
uint16_t get_hash_hex_length(enum hash_function_ids hash_id);

#endif