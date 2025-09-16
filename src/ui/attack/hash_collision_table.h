/**
 * \file            hash_collision_table.h
 * \brief           Header file for hash_collision_table.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HASH_COLLISION_TABLE_H
#define HASH_COLLISION_TABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashNode {
    struct HashNode* next; ///< Pointer to the next node in the linked list
    char* hash_hex;        ///< The hash value of the input
    char* input;           ///< The input that generated the hash
} hash_node_t;

typedef struct {
    hash_node_t** buckets; ///< Pointers to the head of linked lists of the start of the hash table
    unsigned int bucket_count; ///< The number of nodes in the linked list
} hash_table_t;

hash_table_t* hash_table_create(size_t bucket_count);
size_t simple_hash(const char* str, size_t bucket_count);
hash_node_t* hash_table_find(hash_table_t* table, const char* hash_hex);
bool hash_table_insert(hash_table_t* table, const char* input, const char* hash_hex);
void hash_table_destroy(hash_table_t* table);

#endif