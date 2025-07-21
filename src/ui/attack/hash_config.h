/**
 * \file            hash_config.h
 * \brief           Header file for hash_config.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

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
    enum hash_function_ids id;        ///< The ID of the hash function
    const char* label;                ///< The label for the hash function
    const unsigned short bits;        ///< The bit size of the hash function
    const char* estimated_collisions; ///< Estimated collisions for the hash function
    const char* space_size;           ///< Space size of the hash function
} hash_config_t;

extern const hash_config_t hash_config[];    ///< Array of hash configurations
extern const unsigned short hash_config_len; ///< The number of hash configurations

struct ListMenuItem* get_hash_config_menu();

hash_config_t get_hash_config_item(enum hash_function_ids id);

uint16_t get_hash_hex_length(enum hash_function_ids hash_id);

#endif