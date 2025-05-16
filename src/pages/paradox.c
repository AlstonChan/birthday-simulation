
#include <ncurses/ncurses.h>

void render_paradox_page(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  // Clear the window before rendering
  werase(win);

  // Resize the window for the paradox page
  wresize(win, max_y - 6, max_x);

  // Center the paradox page window
  mvwin(win, 4, 0);

  box(win, 0, 0);

  mvwprintw(win, 0, 3, "[ Birthday Paradox Simulation ]");

  // Refresh the window to show the paradox page
  wrefresh(win);

  // Wait for user input before closing the system info window
  wgetch(win);

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);
}