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

typedef enum {
    ERROR_NONE = 0,
    ERROR_MEMORY_ALLOCATION,
    ERROR_HASH_COMPUTATION,
    ERROR_HASH_TABLE_INSERT,
    ERROR_RESULT_MUTEX_NOT_ALLOCATED,
    ERROR_HASH_TABLE_MUTEX_NOT_ALLOCATED
} error_type_t;

typedef struct {
    volatile bool has_error;        // Atomic flag
    unsigned int worker_id;         // Which worker had the error
    error_type_t error_type;
    char error_message[368];        // Human-readable error description
    char error_location[256];       // File:line or function name
    GMutex* error_mutex;            // Protect writes to error info
} thread_error_info_t;

typedef struct HashCollisionSimulationResult {
    int attempts_made; ///< The number of attempts made to find a collision or no collision
    bool collision_found;       ///< Whether a collision was found or not
    char* collision_input_1;    ///< The first input that caused a collision
    char* collision_input_2;    ///< The second input that caused a collision
    char* collision_hash_hex;   ///< The hash value of the collision inputs
} hash_collision_simulation_result_t;

typedef struct HashCollisionContext {
    enum hash_function_ids hash_id;
    hash_table_t* shared_table;
    GMutex* table_mutex;

    int cancel; ///< Flag to signal cancellation to worker threads
    int remaining_workers; ///< Count of remaining active worker threads, used to determine when all threads have completed

    bool collision_found;
    hash_collision_simulation_result_t* result;
    GMutex* result_mutex;

    thread_error_info_t* error_info;
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
thread_error_info_t* error_info_create(void);
void register_thread_error(hash_collision_context_t* ctx, 
                          unsigned int worker_id,
                          error_type_t error_type,
                          const char* error_message,
                          const char* error_location);
const char* error_type_to_string(error_type_t type);

#define REGISTER_ERROR(ctx, worker_id, type, msg) \
    register_thread_error(ctx, worker_id, type, msg, __FILE__ ":" G_STRINGIFY(__LINE__))

#define REGISTER_ERROR_FUNC(ctx, worker_id, type, msg) \
    register_thread_error(ctx, worker_id, type, msg, __FUNCTION__)

#endif