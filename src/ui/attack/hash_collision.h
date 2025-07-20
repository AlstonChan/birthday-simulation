#ifndef HASH_COLLISION_H
#define HASH_COLLISION_H

#include <ctype.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdint.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "hash_config.h"

#include "../../utils/hash_function.h"
#include "../../utils/utils.h"
#include "../form.h"
#include "../layout.h"
#include "../error.h"
#include "../footer.h"
#include "../header.h"
#include "../../utils/resize.h"

/**
 * @brief Render the hash collision page in the given window that allows
 * simulation of hash function to demonstrate collisions/no collisions.
 * 
 * @param content_win The window to render the attack page on
 * @param header_win The window to render the header content, normally for 
 * the args of header_render
 * @param footer_win The window to render the footer content, normally for 
 * the args of footer_render
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 * @param hash_id The ID of the hash function to simulate collisions for. This should
 * be one of the enum hash_function_ids values defined in hash_config.h.
 */
void render_hash_collision_page(WINDOW *content_win, WINDOW *header_win, WINDOW *footer_win, int max_y, int max_x, enum hash_function_ids hash_id);

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