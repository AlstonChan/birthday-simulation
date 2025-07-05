#ifndef HASH_COLLISION_H
#define HASH_COLLISION_H

#include <ncurses/ncurses.h>

#include "hash_config.h"

/**
 * @brief Render the hash collision page in the given window that allows
 * simulation of hash function to demonstrate collisions/no collisions.
 * 
 * @param win The content window to render the hash collision page in.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 * @param hash_id The ID of the hash function to simulate collisions for. This should
 * be one of the enum hash_function_ids values defined in hash_config.h.
 */
void render_hash_collision_page(WINDOW *win, int max_y, int max_x, enum hash_function_ids hash_id);

typedef struct HashCollisionSimulationResult {
    enum hash_function_ids id;     ///< The ID of the hash function
    unsigned int attempts_made;             ///< The number of attempts made to find a collision or no collision
    bool collision_found;          ///< Whether a collision was found or not
    char *collision_input_1;       ///< The first input that caused a collision
    char *collision_input_2;       ///< The second input that caused a collision
    char *collision_hash_hex;      ///< The hash value of the collision inputs
} hash_collision_simulation_result_t;

typedef struct HashNode {
    struct HashNode *next;         ///< Pointer to the next node in the linked list
    char *hash_hex;              ///< The hash value of the input
    char *input;              ///< The input that generated the hash
} hash_node_t;

typedef struct {
    hash_node_t **buckets;     ///< Pointers to the head of linked lists of the start of the hash table
    unsigned int bucket_count;         ///< The number of nodes in the linked list
} hash_table_t;

#endif 