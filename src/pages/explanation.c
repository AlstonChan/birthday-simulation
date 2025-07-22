/**
 * \file            explanation.c
 * \brief           A documentation page for both the application and the core
 *                  theme of this application -- birthday paradox and birthday
 *                  attack.
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "explanation.h"

static const char const* s_explanation_page_title = "[ Birthday Paradox Information ]";

/**
 * \brief          Renders the explanation page in the given window.
 *
 * \param[in]      content_win The window to render the explanation page on
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
render_explanation_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int* max_y,
                        int* max_x) {
    if (content_win == NULL || header_win == NULL || footer_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_explanation_page is null");
    }

    COORD win_size;

    bool nodelay_modified = false;
    if (!is_nodelay(content_win)) {
        nodelay(content_win, TRUE);
        nodelay_modified = true; // Track if we modified nodelay
    }

    // Clear the window before rendering
    werase(content_win);

    // Resize the window for the explanation page
    wresize(content_win, *max_y - 6, *max_x);

    // Center the explanation page window
    mvwin(content_win, 4, 0);

    box(content_win, 0, 0);

    unsigned short title_len = strlen(s_explanation_page_title);
    mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_explanation_page_title);

    // Refresh the window to show the explanation page
    wrefresh(content_win);

    int ch;
    while ((ch = wgetch(content_win)) != KEY_F(2)) {
        if (check_console_window_resize_event(&win_size)) {
            int resize_result = resize_term(win_size.Y, win_size.X);
            if (resize_result != OK) {
                render_full_page_error(
                    content_win, 0, 0,
                    "Unable to resize the UI to the terminal new size. Resize failure.");
            }
            // mvwprintw(stdscr, 0, 0, "%d-%d", win_size.Y, win_size.X); // For debugging purpose only
            clear();
            wclear(content_win);
            wclear(footer_win);

            refresh();

            *max_y = win_size.Y;
            *max_x = win_size.X;

            wresize(content_win, *max_y - BH_LAYOUT_PADDING, *max_x);

            box(content_win, 0, 0);
            mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_explanation_page_title);

            header_render(header_win);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y - 2, *max_x);

            wrefresh(content_win);
        }
    }

    if (nodelay_modified) {
        nodelay(content_win, FALSE); // Restore nodelay to true
    }

    werase(content_win);
    wrefresh(content_win);
}