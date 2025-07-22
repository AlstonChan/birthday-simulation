/**
 * \file            paradox.c
 * \brief           A page that will execute the math of birthday paradox and
 *                  simulate how it works over a user defined number of times
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "paradox.h"

static const char const* s_paradox_page_title = "[ Birthday Paradox Simulation ]";

/**
 * \brief          Renders the paradox page in the given window.
 *
 * \param[in]      content_win The window to render the paradox page on
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
render_paradox_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int* max_y,
                    int* max_x) {
    // If any of the window supplied is NULL, it is a mistake
    if (content_win == NULL || header_win == NULL || footer_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_paradox_page is null");
    }

    curs_set(1);                // Show the cursor
    nodelay(content_win, TRUE); // Make getch() non-blocking
    COORD win_size;

    // Clear the window before rendering
    werase(content_win);

    // Resize the window for the paradox page
    wresize(content_win, *max_y - BH_LAYOUT_PADDING, *max_x);

    // Center the paradox page window
    mvwin(content_win, 4, 0);
    box(content_win, 0, 0);

    unsigned short title_len = strlen(s_paradox_page_title);
    mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_paradox_page_title);

    paradox_form_init(content_win, *max_y, *max_x); // Initialize the paradox form
    FORM* paradox_form =
        paradox_form_render(content_win, *max_y, *max_x); // Render the paradox form

    // Refresh the window to show the paradox page
    wrefresh(content_win);
    pos_form_cursor(paradox_form); // Position the cursor for the current field

    double collision_probability = -1, simulated_runs_results = -1;
    int ch;
    while ((ch = wgetch(content_win)) != KEY_F(2)) {
        paradox_form_handle_input(content_win, ch, &collision_probability, &simulated_runs_results);

        // Check if terminal was resized
        if (check_console_window_resize_event(&win_size)) {
            int resize_result = resize_term(win_size.Y, win_size.X);
            if (resize_result != OK) {
                render_full_page_error(
                    content_win, 0, 0,
                    "Unable to resize the UI to the terminal new size. Resize failure.");
            }
            // mvwprintw(win, 0, 0, "%d-%d", win_size.Y, win_size.X); // For debugging purpose only

            *max_y = win_size.Y;
            *max_x = win_size.X;

            wresize(content_win, *max_y - BH_LAYOUT_PADDING, *max_x);

            wclear(content_win);
            wclear(footer_win);

            box(content_win, 0, 0);
            mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_paradox_page_title);
            header_render(header_win);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y - 2, *max_x);
            paradox_form_restore(content_win, *max_y - BH_LAYOUT_PADDING, *max_x,
                                 collision_probability, simulated_runs_results);

            wrefresh(content_win);
        } else {
            wrefresh(content_win);
        }
    }

    paradox_form_destroy();

    curs_set(0);                 // Hide the cursor
    nodelay(content_win, FALSE); // Make getch() blocking

    // Clear the window after user input
    werase(content_win);

    // Refresh the window to show the changes
    wrefresh(content_win);
}