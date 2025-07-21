#include "system_info.h"

static const char const* s_system_info_page_title = "[ System Information ]";
static int s_win_rows = 15, s_win_cols = 40;

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/
/**
 * @brief Render the system info like application version
 * and dependencies version. This function does not
 * refresh the window after printing
 *
 * @param win The content to render the info at
 */
static void
render_info(WINDOW* win) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0, "The window passed to render_info is null");
    }

    unsigned short title_len = strlen(s_system_info_page_title);
    mvwprintw(win, 0, (s_win_cols - title_len) / 2, s_system_info_page_title);

    mvwprintw(win, 2, 2, "Program Version: %s", PROGRAM_VERSION_STRING);

    // Display dependency versions
    mvwprintw(win, 4, 2, "Dependencies:");
    mvwprintw(win, 5, 2, "- C Standard Version: %d", __STDC_VERSION__);
    mvwprintw(win, 6, 2, "- Ncurses Version: %d.%d.%d", NCURSES_VERSION_MAJOR,
              NCURSES_VERSION_MINOR, NCURSES_VERSION_PATCH);

    unsigned long version_num = OPENSSL_VERSION_NUMBER;
    mvwprintw(win, 7, 2, "- OpenSSL Version: %lu.%lu.%lu", (version_num >> 28) & 0xFF,
              (version_num >> 20) & 0xFF, (version_num >> 4) & 0xFF);
}

/**
 * @brief Calculate the window's y and x coordinate given the stdscr maximum
 * width and height
 *
 * @param max_y stdscr's maximum row
 * @param max_x stdscr's maximum col
 * @param win_y The value to store the window's y coordinate
 * @param win_x The value to store the window's x coordinate
 */
static void
calculate_win_size(int max_y, int max_x, int* win_y, int* win_x) {
    *win_y = (max_y - s_win_rows) / 2;
    *win_x = (max_x - s_win_cols) / 2;
}

/****************************************************************
                       EXTERNAL FUNCTION
****************************************************************/

void
render_system_info(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int max_y,
                   int max_x) {
    if (content_win == NULL || header_win == NULL || footer_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_system_info is null");
    }

    bool nodelay_modified = false;
    if (!is_nodelay(content_win)) {
        nodelay(content_win, TRUE);
        nodelay_modified = true; // Track if we modified nodelay
    }

    int win_y, win_x;
    calculate_win_size(max_y, max_x, &win_y, &win_x);

    werase(content_win);                          // Clear the window before rendering
    wresize(content_win, s_win_rows, s_win_cols); // Resize the window for the system info

    mvwin(content_win, win_y, win_x); // Center the system info window
    box(content_win, 0, 0);
    render_info(content_win);

    wrefresh(content_win);

    COORD win_size;

    // Wait for user input before closing the system info window
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

            max_y = win_size.Y;
            max_x = win_size.X;

            calculate_win_size(max_y, max_x, &win_y, &win_x);

            mvwin(content_win, win_y, win_x);
            box(content_win, 0, 0);
            render_info(content_win);

            header_render(header_win);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y - 2, max_x);

            wrefresh(content_win);
        }
    }

    if (nodelay_modified) {
        nodelay(content_win, FALSE); // Restore nodelay to true
    }

    werase(content_win);
    wrefresh(content_win);
}