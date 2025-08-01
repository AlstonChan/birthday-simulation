/**
 * \file            main.c
 * \brief           The entry point of the birthday simulation paradox
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <locale.h>
#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "ui/error.h"
#include "ui/footer.h"
#include "ui/home/main_menu.h"
#include "ui/layout.h"
#include "utils/resize.h"
#include "utils/utils.h"

int
main() {
    setlocale(LC_ALL, ""); // Set the locale to the user's default (utf-8)
    initscr();             // Initialize ncurses

    cbreak();              // Disable line buffering
    noecho();              // Don't echo user input
    keypad(stdscr, TRUE);  // Enable special keys (like arrow keys)
    nodelay(stdscr, TRUE); // Make getch() non-blocking
    curs_set(0);           // Hide the cursor

    if (init_color_pairs() != 0) {
        render_full_page_error(stdscr, 0, 0, "Your terminal does not supports colours");
        return 1;
    }

#if _WIN32
    if (GetConsoleOutputCP() != 65001) {
        render_full_page_error(
            stdscr, 0, 0, "Your terminal does not supports UTF-8. Exit error screen to continue.");
        clear();
    }
#endif

    COORD win_size;

    int max_y, max_x;               // Store the stdscr dimensions
    getmaxyx(stdscr, max_y, max_x); // Get initial window size

    WINDOW* header_win = newwin(2, max_x, 0, 0);
    WINDOW* footer_win = newwin(2, max_x, max_y - 2, 0);
    WINDOW* content_win = newwin(max_y - 4, max_x, 2, 0);
    keypad(content_win, TRUE); // Enable special keys in the menu window

    main_menu_init(content_win);
    MENU* main_menu = main_menu_render(content_win, max_y, max_x);

    footer_render(footer_win, 0, 0);

    int char_input;
    bool is_done = false;
    while ((char_input = getch()) != KEY_F(1) && !is_done) {
        int selected_item_index = item_index(current_item(main_menu));
        int current_frame_y = max_y, current_frame_x = max_x;

        switch (char_input) {
            case KEY_DOWN:
            case '\t': // Tab key
                // If the user presses down on the last item, wrap around to the first item
                if (selected_item_index == main_menu_choices_len - 1) {
                    menu_driver(main_menu, REQ_FIRST_ITEM);
                } else {
                    menu_driver(main_menu, REQ_DOWN_ITEM);
                }
                break;
            case KEY_UP:
            case KEY_BTAB: // Shift + Tab key
                // If the user presses up on the first item, wrap around to the last item
                if (selected_item_index == 0) {
                    menu_driver(main_menu, REQ_LAST_ITEM);
                } else {
                    menu_driver(main_menu, REQ_UP_ITEM);
                }
                break;
            case KEY_ENTER:
            case 10: // Enter key
                switch (selected_item_index) {
                    case PARADOX_WIN:
                    case ATTACK_WIN:
                    case EXPLANATION_WIN:
                    case SYSTEM_INFO_WIN:
                        page_layout_render(header_win, footer_win, content_win, &max_y, &max_x,
                                           selected_item_index);
                        break;
                    case 4: is_done = true; break;
                    default:
                        // This case should never be reached, exit the loop
                        render_full_page_error(
                            stdscr, 0, 0, "Unreachable switch statement reached for the main loop");
                        break;
                }
                break;
        }

        // If the page (other than the main page) is being resized, the control pf resizing the windows
        // falls under each while loop of the page. So the pages own while loop will handle the resize
        // and get access to the correct max_y and max_x. However, the parent page will not have access
        // to such info, thus when navigating back one page to the parent page, the content will be
        // screwed as the resize at the children's page does not affect the parent. To solve this, a
        // pointer of max_y and max_x is passed instead, and the child page will update the parent's
        // max_y and max_x and the following condition evaluate if the boundary has been updated and
        // then make the necessary repaint of window
        bool frame_has_resized = false;
        if (current_frame_y != max_y || current_frame_x != max_x) {
            win_size.Y = max_y;
            win_size.X = max_x;
            frame_has_resized = true;
        }

        // Check if terminal was resized
        if (check_console_window_resize_event(&win_size) || frame_has_resized) {
            int resize_result = resize_term(win_size.Y, win_size.X);
            if (resize_result != OK) {
                render_full_page_error(
                    stdscr, 0, 0,
                    "Unable to resize the UI to the terminal new size. Resize failure.");
            }
            // mvwprintw(stdscr, 0, 0, "%d-%d", win_size.Y, win_size.X); // For debugging purpose only

            if (!frame_has_resized) {
                max_y = win_size.Y;
                max_x = win_size.X;
            }

            clear();
            wclear(footer_win);

            main_menu_restore(content_win, win_size.Y, win_size.X);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y, win_size.X);

            refresh();

            if (frame_has_resized) {
                frame_has_resized = false;
            }
        } else {
            wrefresh(content_win);
        }
    }

    main_menu_destroy();
    endwin(); // End ncurses mode
    return 0;
}
