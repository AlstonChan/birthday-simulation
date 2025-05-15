#include <ncurses/ncurses.h>

/**
 * @brief Render the system information page given a window.
 *
 * @param win The window to render the system information in. If NULL, uses stdscr.
 * @param max_y The maximum y-coordinate of the parent window.
 * @param max_x The maximum x-coordinate of the parent window.
 */
void render_system_info(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  // Clear the window before rendering
  werase(win);

  wrefresh(win);

  // Resize the window for the system info
  wresize(win, 10, 40);

  // Center the system info window
  mvwin(win, (max_y - 10) / 2, (max_x - 40) / 2);

  // Display system information
  mvwprintw(win, 1, 1, "System Information");
  mvwprintw(win,
            3,
            1,
            "Ncurses version: %d.%d.%d",
            NCURSES_VERSION_MAJOR,
            NCURSES_VERSION_MINOR,
            NCURSES_VERSION_PATCH);

  // Refresh the window to show the system info
  wrefresh(win);

  // Wait for user input before closing the system info window
  wgetch(win);

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);
}