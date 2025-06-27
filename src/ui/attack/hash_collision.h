#ifndef HASH_COLLISION_H
#define HASH_COLLISION_H

#include <ncurses/ncurses.h>

#include "hash_config.h"

void render_hash_collision_page(WINDOW *win, int max_y, int max_x, enum hash_function_ids hash_id);

void hash_collision_form_init(WINDOW *win);
FORM *hash_collision_form_render(WINDOW *win, int max_y, int max_x);
void hash_form_handle_input(WINDOW *win, int ch);

#endif 