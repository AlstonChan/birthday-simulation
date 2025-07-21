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
 * @brief The amount of rows to pad the menu subwindow with
 * the menu items.
 *
 */
#define MENU_PADDING_Y 2

/**
 * @brief Initializes the a list menu.
 *
 * @param win The window to display the menu in
 * @param choices The choices for the menu.
 * @param choices_len The number of choices in the menu.
 * @param choices_items The ncurses menu items created from the choices.
 * @param menu The menu to initialize.
 * @param sub_win The sub-window for the menu.
 */
void list_menu_init(WINDOW* win, const struct ListMenuItem choices[], unsigned short choices_len,
                    ITEM*** choices_items, MENU** menu, WINDOW** sub_win);

/**
 * @brief Renders the menu navigation text in the specified window.
 *
 * @param win The window to render the navigation text in
 * @param y The y-coordinate where the navigation text will be rendered.
 * @param x The x-coordinate where the navigation text will be rendered. Negative
 * value indicates that the text will be centered in the window.
 * @param hide_exit_text If true, the exit text will not be rendered
 */
void list_menu_navigation_render(WINDOW* win, int y, int x, bool hide_exit_text);

#endif