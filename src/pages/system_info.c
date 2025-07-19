#include "system_info.h"

void render_system_info(WINDOW *win, int max_y, int max_x) {
  if (win == NULL) {
    render_full_page_error_exit(stdscr, 0, 0, "The window passed to render_system_info is null");
  }

  werase(win);                                    // Clear the window before rendering
  wresize(win, 10, 40);                           // Resize the window for the system info
  mvwin(win, (max_y - 10) / 2, (max_x - 40) / 2); // Center the system info window
  box(win, 0, 0);

  // Display system information
  mvwprintw(win, 0, 3, "[ System Information ]");
  mvwprintw(win, 2, 2, "Program Version: %s", PROGRAM_VERSION_STRING);

  // Display dependency versions
  mvwprintw(win, 4, 2, "Dependencies:");
  mvwprintw(win, 5, 2, "- C Standard Version: %d", __STDC_VERSION__);
  mvwprintw(win,
            6,
            2,
            "- Ncurses Version: %d.%d.%d",
            NCURSES_VERSION_MAJOR,
            NCURSES_VERSION_MINOR,
            NCURSES_VERSION_PATCH);

  unsigned long version_num = OPENSSL_VERSION_NUMBER;
  mvwprintw(win,
            7,
            2,
            "- OpenSSL Version: %lu.%lu.%lu",
            (version_num >> 28) & 0xFF,
            (version_num >> 20) & 0xFF,
            (version_num >> 4) & 0xFF);

  // Refresh the window to show the system info
  wrefresh(win);

  // Wait for user input before closing the system info window
  int ch;
  while ((ch = wgetch(win)) != KEY_F(2)) {
  }

  werase(win);
  wrefresh(win);
}