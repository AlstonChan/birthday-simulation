#ifndef HASH_MENU_H
#define HASH_MENU_H

#include <ncurses/menu.h>
#include <ncurses/ncurses.h>

#include "../menu.h"

extern const struct ListMenuItem hash_menu_choices[];
extern const unsigned short hash_menu_choices_len;

MENU *hash_menu_get();

/**
 * @brief Initializes the has algo selection menu.
 *
 * @param win The window to display the menu in. If NULL, uses stdscr.
 * @return true if the menu was successfully initialized, false if it was already initialized.
 */
bool hash_menu_init(WINDOW *win);


/**
 * @brief Renders the menu in the specified window.
 *
 * @param win The window to render the menu in. If NULL, uses stdscr.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 * @return MENU* The rendered menu.
 */
MENU *hash_menu_render(WINDOW *win, int max_y, int max_x);

/**
 * @brief Erases the menu from the window. So that the window can
 * be used for other purposes.
 *
 */
void hash_menu_erase();

/**
 * @brief Restores the menu to the window. This is useful
 * after the menu has been erased and you want to
 * display it again.
 *
 */
void hash_menu_restore(WINDOW *win, int max_y, int max_x);

/**
 * @brief Destroys the menu and frees allocated memory.
 *
 */
void hash_menu_destroy();

#endif
