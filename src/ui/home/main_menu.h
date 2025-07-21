#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../ui/error.h"
#include "../../utils/utils.h"
#include "../art.h"
#include "../menu.h"

extern const struct ListMenuItem main_menu_choices[];
extern const unsigned short main_menu_choices_len;

void main_menu_init(WINDOW* win);

MENU* main_menu_get();

MENU* main_menu_render(WINDOW* win, int max_y, int max_x);

void main_menu_erase();

void main_menu_restore(WINDOW* win, int max_y, int max_x);

void main_menu_destroy();

#endif