#include <ctype.h>
#include <math.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "explanation.h"

static const char const* s_explanation_page_title = "[ Birthday Paradox Information ]";

/**
 * @brief Renders the explanation page in the given window.
 * If no window is provided, it will use stdscr.
 *
 * @param win The window to render the explanation page in. If NULL, uses stdscr.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 */
void
render_explanation_page(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        win = stdscr; // Use stdscr if no window is provided
    }

    // Clear the window before rendering
    werase(win);

    // Resize the window for the explanation page
    wresize(win, max_y - 6, max_x);

    // Center the explanation page window
    mvwin(win, 4, 0);

    box(win, 0, 0);

    unsigned short title_len = strlen(s_explanation_page_title);
    mvwprintw(win, 0, (max_x - title_len) / 2, s_explanation_page_title);

    // Refresh the window to show the explanation page
    wrefresh(win);

    int ch;
    while ((ch = wgetch(win)) != KEY_F(2)) {}

    // Clear the window after user input
    werase(win);

    // Refresh the window to show the changes
    wrefresh(win);
}