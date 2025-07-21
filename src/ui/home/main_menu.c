/**
 * \file            main_menu.c
 * \brief           The main menu render function for the home page
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "main_menu.h"

const struct ListMenuItem main_menu_choices[] = {
    {"Birthday Paradox Simulation", NULL},
    {"Attack Simulation", NULL},
    {"Explanation", NULL},
    {"System Info", NULL},
    {"Exit", NULL},
};
const unsigned short main_menu_choices_len = ARRAY_SIZE(main_menu_choices);

static ITEM** s_main_menu_choices_items = NULL;
static MENU* s_main_menu = NULL;
static WINDOW* s_main_menu_sub_win = NULL;

/**
 * \brief          Gets the current menu.
 *
 * \return         The current menu.
 */
MENU*
main_menu_get() {
    return s_main_menu;
}

/**
 * \brief          Initializes the main menu.
 *
 * \param[in]      win The window to display the menu in.
 */
void
main_menu_init(WINDOW* win) {
    list_menu_init(win, main_menu_choices, main_menu_choices_len, &s_main_menu_choices_items,
                   &s_main_menu, &s_main_menu_sub_win);
}

/**
 * \brief          Renders the menu in the specified window.
 *
 * \param[in]      win The window to render the menu in. If NULL, uses stdscr.
 * \param[in]      max_y The maximum y-coordinate of the parent window (stdscr).
 * \param[in]      max_x The maximum x-coordinate of the parent window (stdscr).
 * \return         The rendered menu.
 */
MENU*
main_menu_render(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0, "The window passed to main_menu_render is null");
    }

    if (!s_main_menu) {
        main_menu_init(win); // Initialize the menu if not already done
    }

    // Resize the window for the menu
    unsigned short main_menu_win_rows = ARRAY_SIZE(main_menu_choices) + 4;
    wresize(win, main_menu_win_rows, 40);

    // Center the menu window
    int y = (max_y - 15) / 2;
    int x = (max_x - 40) / 2;
    mvwin(win, y, x);

    box(win, 0, 0);
    print_in_middle(win, 0, 0, 40, " Main Menu ", COLOR_PAIR(BH_MAIN_COLOR_PAIR));

    art_text_center_render(stdscr, max_y, max_x);

    // Render the menu navigation text
    list_menu_navigation_render(stdscr, y + main_menu_win_rows + 1, -1, false);

    wrefresh(win);

    // If the menu window is not the standard screen, refresh it
    if (win != stdscr) {
        refresh();
    }

    return s_main_menu;
}

/**
 * \brief          Erases the menu from the window. So that the window can
 *                 be used for other purposes.
 *
 */
void
main_menu_erase() {
    if (!s_main_menu) {
        return;
    }

    unpost_menu(s_main_menu); // Erase the menu from the window
}

/**
 * \brief          Restores the menu to the window. This is useful
 *                 after the menu has been erased and you want to
 *                 display it again.
 * 
 * \param[in]      win The window to render the menu in. If NULL, uses stdscr.
 * \param[in]      max_y The maximum y-coordinate of the parent window (stdscr).
 * \param[in]      max_x The maximum x-coordinate of the parent window (stdscr).
 */
void
main_menu_restore(WINDOW* win, int max_y, int max_x) {
    if (!s_main_menu) {
        return;
    }

    post_menu(s_main_menu);              // Post the menu to the window
    main_menu_render(win, max_y, max_x); // Render the menu in the window
    wrefresh(s_main_menu_sub_win);
}

/**
 * \brief          Destroys the menu and frees allocated memory.
 *
 */
void
main_menu_destroy() {
    if (!s_main_menu) {
        return;
    }

    main_menu_erase();      // Erase the menu from the window
    free_menu(s_main_menu); // Free the memory allocated for the menu

    for (unsigned short i = 0; i < ARRAY_SIZE(main_menu_choices); ++i) {
        free_item(s_main_menu_choices_items[i]); // Free the memory allocated for each item
    }
    free(s_main_menu_choices_items[ARRAY_SIZE(main_menu_choices)]); // Free the NULL terminator

    s_main_menu = NULL;               // Set the menu pointer to NULL
    s_main_menu_choices_items = NULL; // Set the items pointer to NULL
}