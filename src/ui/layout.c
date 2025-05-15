#include <ncurses/ncurses.h>
#include <string.h>

void content_layout_render(WINDOW *header_win, WINDOW *footer_win) {
  bool is_header_win_null = (header_win == NULL), is_footer_win_null = (footer_win == NULL);

  if (!is_header_win_null) {
    int max_y, max_x;
    getmaxyx(header_win, max_y, max_x); // Get the size of the header window

    werase(header_win);
    mvwprintw(header_win, 1, 2, "[F2]: Back");

    // Set the attribute to bold and cyan
    wattron(header_win, A_BOLD | COLOR_PAIR(1));

    const char const *program_name = "Birthday Simulation";
    const unsigned short program_name_len = strlen(program_name);
    mvwprintw(header_win, 1, max_x - program_name_len - 2, program_name);

    wattroff(header_win, A_BOLD | COLOR_PAIR(1)); // Turn off the attributes
    wrefresh(header_win);
  }

  if (!is_footer_win_null) {
    int max_y, max_x;
    getmaxyx(footer_win, max_y, max_x); // Get the size of the footer window

    werase(footer_win);

    const char const *license_text = "2025 Chan Alston - MPL 2.0";
    const unsigned short license_text_len = strlen(license_text);

    wattron(header_win, A_UNDERLINE);
    mvwprintw(footer_win, 1, (max_x - license_text_len) / 2, license_text);
    wattroff(header_win, A_UNDERLINE);

    wrefresh(footer_win);
  }
}