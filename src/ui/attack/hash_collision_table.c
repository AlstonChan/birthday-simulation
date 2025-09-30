/**
 * \file            hash_collision_table.c
 * \brief           Hash table implementation for hash collision detection
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "hash_collision_table.h"

/**
 * \brief          Create a new hash table with the specified number of buckets.
 *                 You should free the returned hash table using `hash_table_destroy`
 *                 when done.
 *
 * \param[in]      bucket_count The number of buckets in the hash table.
 * \return         A pointer to the newly created hash table, or NULL on memory
 *                 allocation failure
 */
hash_table_t*
hash_table_create(size_t bucket_count) {
    // First, allocate memory for the hash table structure
    hash_table_t* table = malloc(sizeof(hash_table_t));
    if (!table) {
        return NULL;
    }

    // Then, allocate memory for the hash_node_t pointers in the buckets array
    table->buckets = calloc(bucket_count, sizeof(hash_node_t*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->bucket_count = bucket_count;
    return table;
}

/**
 * \brief          Uses djb2 algorithm to compute a simple hash for the given string.
 *
 * \param[in]      str The string to hash.
 * \param[in]      bucket_count The number of buckets in the hash table.
 * \return         size_t
 */
size_t
simple_hash(const char* str, size_t bucket_count) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {               // Loops auto terminates at null character
        hash = ((hash << 5) + hash) + c; // Multiply by 33 and add the current character
    }
    return hash % bucket_count;
}

/**
 * \brief          Finds an entry in the hash table by its hash value.
 *                 This function searches for a hash value in the hash table and returns the corresponding
 *                 hash_node_t if found, or NULL if not found.
 *
 * \param[in]      table The hash table to search in.
 * \param[in]      hash_hex The hexadecimal string representation of the hash to find.
 * \return         A pointer to the hash_node_t if found, or NULL if not found.
 */
hash_node_t*
hash_table_find(hash_table_t* table, const char* hash_hex) {
    size_t bucket = simple_hash(hash_hex, table->bucket_count);
    hash_node_t* entry = table->buckets[bucket];

    while (entry) {
        if (strcmp(entry->hash_hex, hash_hex) == 0) {
            return entry; // Found the entry with the matching hash
        }
        entry = entry->next; // Move to the next node in the linked list to continue searching
    }
    return NULL;
}

/**
 * \brief          Inserts a new entry into the hash table.
 *                 This function creates a new hash_node_t, initializes it with the input and hash_hex,
 *                 and inserts it into the appropriate bucket in the hash table.
 *
 * \param[in]      table The hash table to insert into.
 * \param[in]      input The input string that generated the hash.
 * \param[in]      hash_hex The hexadecimal string representation of the hash.
 * \return         true if the insertion was successful, false otherwise (e.g., memory allocation failure).
 */
bool
hash_table_insert(hash_table_t* table, const char* input, const char* hash_hex) {
    size_t bucket = simple_hash(hash_hex, table->bucket_count);

    hash_node_t* entry = malloc(sizeof(hash_node_t));
    if (!entry) {
        return false;
    }

    entry->input = strdup(input);
    entry->hash_hex = strdup(hash_hex);
    if (!entry->input || !entry->hash_hex) {
        free(entry->input);
        free(entry->hash_hex);
        free(entry);
        return false;
    }

    entry->next = table->buckets[bucket];
    table->buckets[bucket] = entry;
    return true;
}

/**
 * \brief          Destroys the hash table and frees all its resources.
 *                 This function iterates through each bucket in the hash table, freeing
 *                 each linked list entry and its associated resources. Finally,
 *                 it frees the buckets array and the hash table itself.
 *
 * \param[in]      table The hash table to destroy.
 *
 */
void
hash_table_destroy(hash_table_t* table) {
    // No table to destroy, return early
    if (!table) {
        return;
    }

    // Loop over each bucket in the hash table
    for (size_t i = 0; i < table->bucket_count; i++) {
        // Gets the head of the linked list for this bucket
        hash_node_t* entry = table->buckets[i];

        // If the bucket is empty, continue to the next bucket
        while (entry) {
            // Get the next entry before freeing the current one
            hash_node_t* next = entry->next;

            // Free the current entry's resources
            free(entry->input);
            free(entry->hash_hex);
            free(entry);

            // Move to the next entry in the linked list
            entry = next;
        }
    }

    // Finally, free the hash table's buckets array and the table itself
    free(table->buckets);
    free(table);
}