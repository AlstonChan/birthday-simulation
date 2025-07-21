#include "header.h"

/**
 * \brief          Render the header of the application
 *
 * \param[in]      header_win The window to render the content at.
 */
void
header_render(WINDOW* header_win) {
    if (header_render == NULL) {
        render_full_page_error_exit(stdscr, 0, 0, "The window passed to header_render is null");
    }

    int max_y, max_x;
    getmaxyx(header_win, max_y, max_x); // Get the size of the header window

    werase(header_win);
    mvwprintw(header_win, 1, 2, "[F2]: Back");

    static const char const* program_name = "Birthday Simulation";
    const unsigned short program_name_len = strlen(program_name);

    // Set the attribute to bold and cyan
    wattron(header_win, A_BOLD | COLOR_PAIR(BH_MAIN_COLOR_PAIR));
    mvwprintw(header_win, 1, max_x - program_name_len - 2, program_name);
    wattroff(header_win, A_BOLD | COLOR_PAIR(BH_MAIN_COLOR_PAIR)); // Turn off the attributes

    wrefresh(header_win);
}