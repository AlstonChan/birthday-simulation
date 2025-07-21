#include "hash_config.h"

const hash_config_t hash_config[] = {
    {HASH_CONFIG_8BIT, "ToyHash8", (unsigned short)8, "~2^4 = 16", "2^8 = 256"},
    {HASH_CONFIG_12BIT, "ToyHash12", (unsigned short)12, "~2^6 = 64", "2^12 = 4096"},
    {HASH_CONFIG_16BIT, "ToyHash16", (unsigned short)16, "~2^8 = 256", "2^16 = 65536"},
    {HASH_CONFIG_RIPEMD160, "RIPEMD-160", (unsigned short)160, "~2^80", "2^160"},
    {HASH_CONFIG_SHA1, "SHA-1", (unsigned short)160, "~2^80", "2^160"},
    {HASH_CONFIG_SHA3_256, "SHA3-256", (unsigned short)256, "~2^128", "2^256"},
    {HASH_CONFIG_SHA256, "SHA-256", (unsigned short)256, "~2^128", "2^256"},
    {HASH_CONFIG_SHA512, "SHA-512", (unsigned short)512, "~2^256", "2^512"},
    {HASH_CONFIG_SHA384, "SHA-384", (unsigned short)384, "~2^192", "2^384"},
    // {HASH_CONFIG_KECCAK256, "Keccak-256", (unsigned short)256, "~2^128", "2^256"},
};

const unsigned short hash_config_len = ARRAY_SIZE(hash_config);

/**
 * \brief          Get the hash config menu object
 *
 * \return         The list menu item, or NULL if memory allocation failed
 */
struct ListMenuItem*
get_hash_config_menu() {
    struct ListMenuItem* hash_config_menu = malloc(hash_config_len * sizeof(struct ListMenuItem));
    if (!hash_config_menu) {
        return NULL;
    }

    for (unsigned short i = 0; i < hash_config_len; i++) {
        hash_config_menu[i].label = hash_config[i].label;

        char str_buffer[4]; // Enough for 3 digits and null terminator
        snprintf(str_buffer, sizeof(str_buffer), "%hu", hash_config[i].bits);

        size_t description_length =
            strlen(str_buffer) + 9; // 2 for parentheses + 1 space + "bits" + 1 for null terminator
        char* description_parentheses = malloc(description_length);
        if (!description_parentheses) {
            return NULL;
        }

        snprintf(description_parentheses, description_length, "(%s bits)", str_buffer);
        hash_config_menu[i].description = description_parentheses;
    }

    return hash_config_menu;
}

/**
 * \brief          Get the hash config item for a specific hash function ID
 *
 * \param[in]      id The ID of the hash function
 * \return         The menu item for the specified hash function
 */
hash_config_t
get_hash_config_item(enum hash_function_ids id) {
    if (id < 0 || id >= hash_config_len) {
        fprintf(stderr, "Invalid hash function ID: %d\n", id);
        exit(EXIT_FAILURE);
    }

    return hash_config[id];
}

/**
 * \brief          Get maximum hash hex string length for a given hash function
 *                 It returns a uint16_t, because the maximum length of a hash function's hex string
 *                 length in bits is definitely small enough to fit within a 16-bit unsigned integer.
 *                 Even like sha-512, has a 512-bit output, which is less than 2^16 - 1 = 65,535.
 *
 * \param[in]      hash_id The ID of the hash function to get the hex length for
 * \return         The maximum length of the hash hex string, including the null terminator
 */
uint16_t
get_hash_hex_length(enum hash_function_ids hash_id) {
    hash_config_t hash_config_item = get_hash_config_item(hash_id);
    return hash_config_item.bits / 4 + 1; // +1 for null terminator
}