/**
 * \file            attack.c
 * \brief           The birthday attack page that will show a menu of hash function to choose
 *                  that will be used to simulate birthday attack
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "attack.h"

static const char* s_attack_page_title = "[ Birthday Attack Demo ]";

/**
 * \brief          Renders the birthday attack page in the given window.
 *
 * \param[in]      content_win The window to render the attack page on
 * \param[in]      header_win The window to render the header content, normally for
 *                 the args of header_render
 * \param[in]      footer_win The window to render the footer content, normally for
 *                 the args of footer_render
 * \param[out]     max_y The maximum height of the screen space that can be rendered. The
 *                 value will be updated when a resize happens
 * \param[out]     max_x The maximum width of the screen space that can be rendered. The
 *                 value will be updated when a resize happens
 */
void
render_attack_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int* max_y,
                   int* max_x) {
    if (content_win == NULL || header_win == NULL || footer_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_attack_page is null");
    }

    bool nodelay_modified = false;
    if (!is_nodelay(content_win)) {
        nodelay(content_win, TRUE);
        nodelay_modified = true; // Track if we modified nodelay
    }

    // Clear the window before rendering
    werase(content_win);

    hash_menu_restore(content_win, *max_y,
                      *max_x); // Restore the menu to the content window if it was erased

    bool hash_menu_init_status = hash_menu_init(content_win); // Initialize the hash menu
    MENU* hash_menu = hash_menu_init_status ? hash_menu_render(content_win, *max_y, *max_x)
                                            : hash_menu_get(); // Render the hash menu

    // unsigned short title_len = strlen(s_attack_page_title);
    // mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_attack_page_title);

    COORD win_size;

    int char_input;
    while ((char_input = wgetch(content_win)) != KEY_F(2)) {
        int selected_item_index = item_index(current_item(hash_menu));
        int current_frame_y = *max_y, current_frame_x = *max_x;

        switch (char_input) {
            case KEY_DOWN:
            case '\t': // Tab key
                // If the user presses down on the last item, wrap around to the first item
                if (selected_item_index == hash_config_len - 1) {
                    menu_driver(hash_menu, REQ_FIRST_ITEM);
                } else {
                    menu_driver(hash_menu, REQ_DOWN_ITEM);
                }
                break;
            case KEY_UP:
            case KEY_BTAB: // Shift + Tab key
                // If the user presses up on the first item, wrap around to the last item
                if (selected_item_index == 0) {
                    menu_driver(hash_menu, REQ_LAST_ITEM);
                } else {
                    menu_driver(hash_menu, REQ_UP_ITEM);
                }
                break;
            case KEY_ENTER:
            case 10: // Enter key
                hash_menu_erase();
                render_hash_collision_page(content_win, header_win, footer_win, max_y, max_x,
                                           selected_item_index);

                // Back to menu after exiting the hash collision page
                hash_menu_restore(content_win, *max_y, *max_x);
        }

        bool frame_has_resized = false;
        if (current_frame_y != *max_y || current_frame_x != *max_x) {
            win_size.Y = *max_y;
            win_size.X = *max_x;
            frame_has_resized = true;
        }

        if (check_console_window_resize_event(&win_size) || frame_has_resized) {
            int resize_result = resize_term(win_size.Y, win_size.X);
            if (resize_result != OK) {
                render_full_page_error(
                    content_win, 0, 0,
                    "Unable to resize the UI to the terminal new size. Resize failure.");
            }
            // mvwprintw(stdscr, 0, 0, "%d-%d", win_size.Y, win_size.X); // For debugging purpose only

            wclear(footer_win);

            clear();
            refresh();

            if (!frame_has_resized) {
                *max_y = win_size.Y;
                *max_x = win_size.X;
            }

            hash_menu_erase();
            hash_menu_restore(content_win, *max_y, *max_x);

            header_render(header_win);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y - 2, *max_x);

            // mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_attack_page_title);

            wrefresh(content_win);

            if (frame_has_resized) {
                frame_has_resized = false;
            }
        }
    }

    hash_menu_destroy();

    if (nodelay_modified) {
        nodelay(content_win, FALSE); // Restore nodelay to true
    }

    // Clear the window after user input
    werase(content_win);

    // Refresh the window to show the changes
    wrefresh(content_win);

    erase();
}
