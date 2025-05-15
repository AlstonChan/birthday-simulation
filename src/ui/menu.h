#ifndef MENU_H
#define MENU_H

#include <ncurses/menu.h>
#include <ncurses/ncurses.h>

extern const char *const main_menu_choices[];
extern const unsigned short main_menu_choices_len;

void main_menu_init(WINDOW *win);
MENU *main_menu_render(WINDOW *win, int max_y, int max_x);
MENU *main_menu_get();
void main_menu_erase();
void main_menu_destroy();

#endif