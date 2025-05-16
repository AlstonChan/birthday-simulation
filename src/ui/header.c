#include <ncurses/ncurses.h>
#include <string.h>

void header_render(WINDOW *header_win) {
  if (header_win == NULL)
    return;

  int max_y, max_x;
  getmaxyx(header_win, max_y, max_x); // Get the size of the header window

  werase(header_win);
  mvwprintw(header_win, 1, 2, "[F2]: Back");

  static const char const *program_name = "Birthday Simulation";
  const unsigned short program_name_len = strlen(program_name);

  // Set the attribute to bold and cyan
  wattron(header_win, A_BOLD | COLOR_PAIR(1));
  mvwprintw(header_win, 1, max_x - program_name_len - 2, program_name);
  wattroff(header_win, A_BOLD | COLOR_PAIR(1)); // Turn off the attributes

  wrefresh(header_win);
}