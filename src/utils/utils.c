#include <limits.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

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
  if (start_x != INT_MAX)
    x = start_x;

  // If a starting y-coordinate is provided, use it.
  if (start_y != INT_MAX)
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

size_t generate_random_input(uint8_t *buffer, size_t min_len, size_t max_len) {
  size_t len = min_len + (rand() % (max_len - min_len + 1));

  for (size_t i = 0; i < len; i++) {
    buffer[i] = 32 + (rand() % 95); // Generate printable ASCII characters (32-126)
    // if (rand() % 3 == 0) {
    //   // 1/3 chance: Generate printable ASCII (32-126)
    //   buffer[i] = 32 + (rand() % 95);
    // } else {
    //   // 2/3 chance: Generate any byte value
    //   buffer[i] = rand() & 0xFF;
    // }
  }

  return len;
}

void binary_to_string(const uint8_t *data, size_t len, char *output) {
  size_t pos = 0;
  for (size_t i = 0; i < len; i++) {
    if (data[i] >= 32 && data[i] <= 126) {
      // Printable ASCII character
      output[pos++] = data[i];
    } else {
      // Non-printable character - show as \xHH
      pos += sprintf(output + pos, "\\x%02X", data[i]);
    }
  }
  output[pos] = '\0';
}

uint8_t init_color_pairs() {
  if (has_colors() == FALSE) { // Check if the terminal supports colors
    endwin();
    fprintf(stderr, "\nBirthday Simulation: Your terminal does not support color\n");
    return 1;
  }
  start_color(); // Start color functionality

  // Initialize color pairs for ncurses
  init_pair(BH_MAIN_COLOR_PAIR, COLOR_CYAN, COLOR_BLACK);         // Main color
  init_pair(BH_ERROR_COLOR_PAIR, COLOR_RED, COLOR_BLACK);         // Error color
  init_pair(BH_SUCCESS_COLOR_PAIR, COLOR_GREEN, COLOR_BLACK);     // Success color
  init_pair(BH_WARNING_COLOR_PAIR, COLOR_YELLOW, COLOR_BLACK);    // Warning color
  init_pair(BH_INFO_COLOR_PAIR, COLOR_CYAN, COLOR_BLACK);         // Info color
  init_pair(BH_HIGHLIGHT_COLOR_PAIR, COLOR_MAGENTA, COLOR_BLACK); // Highlight color
  return 0;
}
