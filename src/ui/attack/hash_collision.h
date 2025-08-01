/**
 * \file            hash_collision.h
 * \brief           Header file for hash_collision.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HASH_COLLISION_H
#define HASH_COLLISION_H

#include <ctype.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "hash_config.h"

#include "../../utils/hash_function.h"
#include "../../utils/resize.h"
#include "../../utils/utils.h"
#include "../error.h"
#include "../footer.h"
#include "../form.h"
#include "../header.h"
#include "../layout.h"

void render_hash_collision_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win,
                                int* max_y, int* max_x, enum hash_function_ids hash_id);

typedef struct HashCollisionSimulationResult {
    enum hash_function_ids id;  ///< The ID of the hash function
    unsigned int attempts_made; ///< The number of attempts made to find a collision or no collision
    bool collision_found;       ///< Whether a collision was found or not
    char* collision_input_1;    ///< The first input that caused a collision
    char* collision_input_2;    ///< The second input that caused a collision
    char* collision_hash_hex;   ///< The hash value of the collision inputs
} hash_collision_simulation_result_t;

typedef struct HashNode {
    struct HashNode* next; ///< Pointer to the next node in the linked list
    char* hash_hex;        ///< The hash value of the input
    char* input;           ///< The input that generated the hash
} hash_node_t;

typedef struct {
    hash_node_t** buckets; ///< Pointers to the head of linked lists of the start of the hash table
    unsigned int bucket_count; ///< The number of nodes in the linked list
} hash_table_t;

#endif