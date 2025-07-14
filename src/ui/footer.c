#include "footer.h"

void footer_render(WINDOW *footer_win, int max_y, int max_x) {
  if (footer_win == NULL)
    return;

  if (max_y == 0) {
    max_y = getmaxy(footer_win);
  }

  if (max_x == 0) {
    max_x = getmaxx(footer_win);
  }

  static const char const *license_text = "2025 Chan Alston - MPL 2.0";
  const unsigned short license_text_len = strlen(license_text);

  wattron(footer_win, A_UNDERLINE);
  mvwprintw(footer_win, 1, (max_x - license_text_len) / 2, license_text);
  wattroff(footer_win, A_UNDERLINE);

  wrefresh(footer_win);
}