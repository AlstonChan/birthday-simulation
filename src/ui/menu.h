#ifndef MENU_H
#define MENU_H

#include <ncurses/menu.h>
#include <ncurses/ncurses.h>

struct ListMenuItem {
    const char *label;
    const char *description;
};


/**
 * @brief Initializes the a list menu.
 *
 * @param win The window to display the menu in. If NULL, uses stdscr.
 * @param choices The choices for the menu.
 * @param choices_len The number of choices in the menu.
 * @param choices_items The ncurses menu items created from the choices.
 * @param menu The menu to initialize.
 * @param sub_win The sub-window for the menu.
 */
void list_menu_init(WINDOW *win, const struct ListMenuItem choices[], unsigned short choices_len, ITEM ***choices_items, MENU **menu,
                    WINDOW **sub_win);

/**
 * @brief Renders the menu navigation text in the specified window.
 * 
 * @param win The window to render the navigation text in. If NULL, uses stdscr.
 * @param y The y-coordinate where the navigation text will be rendered.
 * @param x The x-coordinate where the navigation text will be rendered. Negative
 * value indicates that the text will be centered in the window.
 */
void list_menu_navigation_render(WINDOW *win, int y, int x);
                    
#endif