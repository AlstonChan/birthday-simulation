#ifndef MENU_H
#define MENU_H

#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "error.h"

struct ListMenuItem {
    const char* label;
    const char* description;
};

/**
 * \brief          The amount of rows to pad the menu subwindow with
 *                 the menu items.
 *
 */
#define MENU_PADDING_Y 2

void list_menu_init(WINDOW* win, const struct ListMenuItem choices[], unsigned short choices_len,
                    ITEM*** choices_items, MENU** menu, WINDOW** sub_win);

void list_menu_navigation_render(WINDOW* win, int y, int x, bool hide_exit_text);

#endif