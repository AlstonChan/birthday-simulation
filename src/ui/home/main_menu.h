#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "../art.h"
#include "../menu.h"

extern const struct ListMenuItem main_menu_choices[];
extern const unsigned short main_menu_choices_len;

/**
 * @brief Initializes the main menu.
 *
 * @param win The window to display the menu in. If NULL, uses stdscr.
 */
void main_menu_init(WINDOW *win);


/**
 * @brief Gets the current menu.
 *
 * @return MENU* The current menu.
 */
MENU *main_menu_get();

/**
 * @brief Renders the menu in the specified window.
 *
 * @param win The window to render the menu in. If NULL, uses stdscr.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 * @return MENU* The rendered menu.
 */
MENU *main_menu_render(WINDOW *win, int max_y, int max_x);

/**
 * @brief Erases the menu from the window. So that the window can
 * be used for other purposes.
 *
 */
void main_menu_erase();

/**
 * @brief Restores the menu to the window. This is useful
 * after the menu has been erased and you want to
 * display it again.
 *
 */
void main_menu_restore(WINDOW *win, int max_y, int max_x);

/**
 * @brief Destroys the menu and frees allocated memory.
 *
 */
void main_menu_destroy();

#endif