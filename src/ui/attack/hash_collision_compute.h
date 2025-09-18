/**
 * \file            hash_collision_compute.h
 * \brief           Header file for hash_collision_compute.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HASH_COLLISION_COMPUTE_H
#define HASH_COLLISION_COMPUTE_H

#include <glib.h>
#include <stdint.h>
#include <stdlib.h>

#include "hash_collision_table.h"
#include "hash_config.h"

#include "../../utils/hash_function.h"
#include "../../utils/utils.h"
#include "../error.h"

GThreadPool* create_hash_attack_pool(int num_threads);

typedef struct HashCollisionSimulationResult {
    unsigned int attempts_made; ///< The number of attempts made to find a collision or no collision
    bool collision_found;       ///< Whether a collision was found or not
    char* collision_input_1;    ///< The first input that caused a collision
    char* collision_input_2;    ///< The second input that caused a collision
    char* collision_hash_hex;   ///< The hash value of the collision inputs
} hash_collision_simulation_result_t;

typedef struct HashCollisionContext {
    enum hash_function_ids hash_id;
    hash_table_t* shared_table;
    GMutex* table_mutex;
    bool* collision_found;
    hash_collision_simulation_result_t* result;
    GMutex* result_mutex;
} hash_collision_context_t;

typedef struct {
    hash_collision_context_t* ctx;
    unsigned int attempts_to_make;
    unsigned int worker_id;
} WorkerData;

void deep_copy_hash_collision_simulation_result(hash_collision_simulation_result_t* dest,
                                                const hash_collision_simulation_result_t* src);
void clear_result_hash_collision_simulation_result(hash_collision_simulation_result_t* res,
                                                   bool free_struct);
void clear_result_hash_collision_context(hash_collision_context_t* ctx, bool free_struct);

#endif