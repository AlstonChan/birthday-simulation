#ifndef ATTACK_H
#define ATTACK_H

#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <string.h>

#include "../ui/attack/hash_collision.h"
#include "../ui/attack/hash_config.h"
#include "../ui/attack/hash_menu.h"


void render_attack_page(WINDOW *win, int max_y, int max_x);

#endif
