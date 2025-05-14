#include <ncurses/ncurses.h>
#include <string.h>

/**
 * @brief Prints a string centered within a specified area of a window.
 *
 * This function calculates the horizontal center of the given width and prints
 * the provided string at that position within the specified window, row,
 * and with the given color attribute. If no window is provided, it defaults
 * to the standard screen.
 *
 * @param win The window to print in. If NULL, defaults to stdscr.
 * @param start_y The starting row (y-coordinate) to print on. If 0, uses the
 * current cursor y-position of the window.
 * @param start_x The starting column (x-coordinate) to begin centering from.
 * If 0, uses the current cursor x-position of the window.
 * @param width The width of the area to center the string within. If 0, defaults
 * to 80 columns.
 * @param string The null-terminated string to print.
 * @param color The color attribute to apply to the printed string (e.g., A_BOLD | COLOR_PAIR(1)).
 */
void print_in_middle(WINDOW *win, unsigned int start_y, unsigned int start_x, unsigned int width,
                     const char *string, chtype color) {
  int length, x, y;
  float temp;

  // If no window is provided, use the standard screen.
  if (win == NULL)
    win = stdscr;

  // Get the current cursor position of the window.
  getyx(win, y, x);

  // If a starting x-coordinate is provided, use it.
  if (start_x != 0)
    x = start_x;

  // If a starting y-coordinate is provided, use it.
  if (start_y != 0)
    y = start_y;

  // If no width is provided, default to 40 columns.
  if (width == 0)
    width = 40;

  // Calculate the length of the string.
  length = (int)strlen(string);

  // Calculate the horizontal offset to center the string.
  temp = (float)(width - length) / 2;
  x = start_x + (int)temp;

  // Apply the color attribute.
  wattron(win, color);

  // Print the string at the calculated position.
  mvwprintw(win, y, x, "%s", string);

  // Turn off the color attribute.
  wattroff(win, color);

  // Refresh the window to display the changes.
  refresh();
}