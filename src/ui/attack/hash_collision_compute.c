/**
 * \file            hash_collision_compute.c
 * \brief           The core computation logic for simulating hash collisions
 *                  using the Birthday Attack algorithm with multithreading support.
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "hash_collision_compute.h"

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Compute the hash for the given input based on the specified hash function ID.
 *
 * \param[in]      hash_id The ID of the hash function to use.
 * \param[in]      input The input data to hash, it should be a pointer to an array of bytes.
 * \param[in]      input_len The length of the input data in bytes.
 * \param[out]     output A pointer to a string where the computed hash will be stored in hexadecimal format.
 * \return         true The hash was computed successfully.
 * \return         false Memory allocation failed or an unsupported hash function ID was provided.
 */
static bool
compute_hash(enum hash_function_ids hash_id, const uint8_t* input, size_t input_len,
             char** output) {
    size_t hash_hex_len = get_hash_hex_length(hash_id);
    *output = malloc(hash_hex_len);
    if (!output) {
        return false;
    }

    switch (hash_id) {
        case HASH_CONFIG_8BIT: {
            uint8_t result = hash_8bit(input, input_len);
            sprintf(*output, "%02X", result);
        } break;
        case HASH_CONFIG_12BIT: {
            uint16_t result = hash_12bit(input, input_len);
            sprintf(*output, "%03X", result);
        } break;
        case HASH_CONFIG_16BIT: {
            uint16_t result = hash_16bit(input, input_len);
            sprintf(*output, "%04X", result);
        } break;
        case HASH_CONFIG_RIPEMD160:
        case HASH_CONFIG_SHA1:
        case HASH_CONFIG_SHA3_256:
        case HASH_CONFIG_SHA256:
        case HASH_CONFIG_SHA512:
        case HASH_CONFIG_SHA384: {
            int openssl_id;

            // First, assign the OpenSSL ID based on the hash_id
            switch (hash_id) {
                case HASH_CONFIG_RIPEMD160: openssl_id = BH_OPENSSL_HASH_RIPEMD160; break;
                case HASH_CONFIG_SHA1: openssl_id = BH_OPENSSL_HASH_SHA1; break;
                case HASH_CONFIG_SHA3_256: openssl_id = BH_OPENSSL_HASH_SHA3_256; break;
                case HASH_CONFIG_SHA256: openssl_id = BH_OPENSSL_HASH_SHA256; break;
                case HASH_CONFIG_SHA512: openssl_id = BH_OPENSSL_HASH_SHA512; break;
                case HASH_CONFIG_SHA384: openssl_id = BH_OPENSSL_HASH_SHA384; break;
                default: free(*output); return false;
            }

            // Then, compute the hash using OpenSSL given the OpenSSL ID
            unsigned char* digest = openssl_hash(input, input_len, openssl_id);
            if (!digest) {
                free(*output);
                return false;
            }

            size_t bin_len = hash_hex_len - 1;          // Exclude null terminator
            *output = bytes_to_hex(digest, bin_len, 1); // Convert to hex string
            (*output)[hash_hex_len - 1] = '\0';         // Null-terminate the string
            if (!*output) {
                free(digest);
                return false; // Memory allocation failed in bytes_to_hex
            }

            free(digest); // Free the binary digest after conversion
        } break;
    }

    return true;
}

/**
 * \brief          The worker function that calculates the hash to find collisions.
 *
 * \param[out]     data specific data for this worker, expected to be a pointer to
 *                 WorkerData struct.
 * \param[out]     user_data data passed to every instance of the worker, expected to be
 *                 NULL.
 */
static void
hash_collision_worker(gpointer data, gpointer user_data) {
    WorkerData* worker = (WorkerData*)data;
    hash_collision_context_t* ctx = worker->ctx;

    g_atomic_int_inc((gint*)&ctx->remaining_workers);

    for (unsigned int attempt = 0; attempt < worker->attempts_to_make; ++attempt) {
        if (g_atomic_int_get((gint*)&ctx->cancel)) {
            break; // Exit if cancellation is requested
        }

        // Check if another worker found collision
        g_mutex_lock(ctx->result_mutex);
        if (*ctx->collision_found) {
            g_mutex_unlock(ctx->result_mutex);
            break;
        }
        g_mutex_unlock(ctx->result_mutex);

        // Step 1: Generate a random input
        uint8_t current_input[32];
        size_t input_len = generate_random_input(current_input, 4, 31);

        // Step 2: Compute the hash
        char* hash_hex = NULL;
        bool compute_success = compute_hash(ctx->hash_id, current_input, input_len, &hash_hex);
        if (!compute_success) {
            free(hash_hex);
            continue;
        }

        // Step 3: Check collision (thread-safe)
        g_mutex_lock(ctx->table_mutex);

        // Double-check collision flag while holding lock
        if (*ctx->collision_found) {
            g_mutex_unlock(ctx->table_mutex);
            free(hash_hex);
            break;
        }

        hash_node_t* existing = hash_table_find(ctx->shared_table, hash_hex);

        if (existing) {
            // Collision found! BIRTHDAY ATTACK SUCCESS: Same hash with different inputs!
            g_mutex_lock(ctx->result_mutex);
            if (!*ctx->collision_found) { // First to find collision
                *ctx->collision_found = true;
                ctx->result->collision_found = true;
                ctx->result->collision_input_1 = strdup(existing->input);
                ctx->result->collision_input_2 =
                    strdup(bytes_to_hex(current_input, input_len, true));
                ctx->result->collision_hash_hex = strdup(hash_hex);
            }
            g_mutex_unlock(ctx->result_mutex);

            g_mutex_unlock(ctx->table_mutex);
            free(hash_hex);
            break;
        } else {
            // Insert new hash
            bool insert_success = hash_table_insert(
                ctx->shared_table, bytes_to_hex(current_input, input_len, true), hash_hex);
            g_mutex_unlock(ctx->table_mutex);

            if (!insert_success) {
                // Handle insertion failure if needed
            }
        }

        free(hash_hex);

        // Update attempts counter
        g_atomic_int_inc((guint*)&ctx->result->attempts_made);
    }

    g_atomic_int_dec_and_test((gint*)&ctx->remaining_workers);
    // Cleanup worker data
    g_free(worker);
}

/**************************************************************
                      EXTERNAL FUNCTIONS
**************************************************************/

/**
 * \brief          Create a glib thread pool for hash collision workers. This is for birthday attack
 *                 simulation to calculate hash collision in parallel.
 *
 * \param[in]      num_threads the number of threads to create in the pool.
 * \return         GThreadPool* A pointer to the created thread pool, or NULL on
 *                 failure.
 */
GThreadPool*
create_hash_attack_pool(int num_threads) {
    GError* error = NULL;
    GThreadPool* pool =
        g_thread_pool_new(hash_collision_worker, // function to be executed by each thread
                          NULL,                  // user data to be passed to the function
                          num_threads,           // maximum number of threads in the pool
                          FALSE, // whether threads are exclusive (TRUE) or shared (FALSE)
                          &error // pointer to a GError to capture any errors
        );

    if (error) {
        char* msg = g_strdup_printf("Failed to create thread pool: %s", error->message);
        render_full_page_error(stdscr, 0, 0, msg);
        g_free(msg);
        g_error_free(error);
        return NULL;
    }

    return pool;
}

/****************************************************************
                        HELPER FUNCTION
****************************************************************/

/**
 * \brief          Perform a deep copy of a hash_collision_simulation_result_t structure.
 *
 *                 This function copies all scalar fields directly and duplicates any dynamically
 *                 allocated strings so that the destination structure owns its own independent
 *                 copies. The caller is responsible for later freeing the strings inside \p dest
 *                 using clear_result_hash_collision_simulation_result().
 *
 * \param[out]     dest Pointer to the destination structure that will receive the copy.
 * \param[in]      src Pointer to the source structure to copy from.
 */
void
deep_copy_hash_collision_simulation_result(hash_collision_simulation_result_t* dest,
                                           const hash_collision_simulation_result_t* src) {
    dest->attempts_made = src->attempts_made;
    dest->collision_found = src->collision_found;

    // Deep copy each string safely
    dest->collision_input_1 = src->collision_input_1 ? strdup(src->collision_input_1) : NULL;
    dest->collision_input_2 = src->collision_input_2 ? strdup(src->collision_input_2) : NULL;
    dest->collision_hash_hex = src->collision_hash_hex ? strdup(src->collision_hash_hex) : NULL;
}

/**
 * \brief          Release memory held by a hash_collision_simulation_result_t instance.
 *
 *                 Frees all dynamically allocated string members inside the structure. Optionally frees
 *                 the structure itself if \p free_struct is TRUE. If the structure is not freed, its
 *                 fields are reset to safe default values so it can be reused.
 *
 * \param[in,out]  res          Pointer to the result structure to clear.
 * \param[in]      free_struct  TRUE to free the structure itself, FALSE to only clear
 *                              its internal data and reset fields.
 */
void
clear_result_hash_collision_simulation_result(hash_collision_simulation_result_t* res,
                                              bool free_struct) {
    free(res->collision_input_1);
    free(res->collision_input_2);
    free(res->collision_hash_hex);

    if (free_struct) {
        free(res);
    } else {
        res->attempts_made = -1;
        res->collision_input_1 = NULL;
        res->collision_input_2 = NULL;
        res->collision_hash_hex = NULL;
        res->collision_found = false;
    }
}

/**
 * \brief          Clean up a hash_collision_context_t instance and its resources.
 *
 *                 This function clears the simulation result (if present), destroys mutexes,
 *                 frees dynamically allocated synchronization primitives and flags, and destroys
 *                 the shared hash table. If \p free_struct is TRUE, the context structure itself
 *                 is also freed; otherwise its pointer fields are set to NULL for safe reuse.
 *
 * \param[in,out]  ctx Pointer to the context structure to clear.
 * \param[in]      free_struct TRUE to free the context structure itself,
 *                             FALSE to only release its internal resources.
 */
void
clear_result_hash_collision_context(hash_collision_context_t* ctx, bool free_struct) {
    if (ctx->result) {
        clear_result_hash_collision_simulation_result(ctx->result, free_struct);
    }

    // Cleanup mutexes
    g_mutex_clear(ctx->table_mutex);
    g_mutex_clear(ctx->result_mutex);

    // free the mutexes
    g_free(ctx->table_mutex);
    g_free(ctx->result_mutex);

    // free the collision found flag
    g_free(ctx->collision_found);

    // Cleanup: Free the hash table and its entries
    hash_table_destroy(ctx->shared_table);

    if (free_struct) {
        free(ctx);
    } else {
        ctx->result_mutex = NULL;
        ctx->table_mutex = NULL;
        ctx->collision_found = NULL;
        ctx->shared_table = NULL;
    }
}