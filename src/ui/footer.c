#include <ncurses/ncurses.h>
#include <string.h>

void footer_render(WINDOW *footer_win) {
  if (footer_win == NULL)
    return;

  static const char const *license_text = "2025 Chan Alston - MPL 2.0";
  const unsigned short license_text_len = strlen(license_text);

  int max_y, max_x;
  getmaxyx(footer_win, max_y, max_x); // Get the size of the footer window

  wattron(footer_win, A_UNDERLINE);
  mvwprintw(footer_win, 1, (max_x - license_text_len) / 2, license_text);
  wattroff(footer_win, A_UNDERLINE);

  wrefresh(footer_win);
}