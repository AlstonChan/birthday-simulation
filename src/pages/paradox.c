#include <ctype.h>
#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../ui/paradox/paradox_form.h"
#include "../utils/utils.h"
#include "paradox.h"

/**
 * @brief The title of the paradox page.
 *
 */
static const char const *paradox_page_title = "[ Birthday Paradox Simulation ]";

/**
 * @brief Renders the paradox page in the given window.
 * If no window is provided, it will use stdscr.
 *
 * @param win The window to render the paradox page in. If NULL, uses stdscr.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 */
void render_paradox_page(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  curs_set(1);        // Show the cursor
  nodelay(win, TRUE); // Make getch() non-blocking

  // Clear the window before rendering
  werase(win);

  // Resize the window for the paradox page
  wresize(win, max_y - 6, max_x);

  // Center the paradox page window
  mvwin(win, 4, 0);

  box(win, 0, 0);

  unsigned short title_len = strlen(paradox_page_title);
  mvwprintw(win, 0, (max_x - title_len) / 2, paradox_page_title);

  paradox_form_init(win);                                      // Initialize the paradox form
  FORM *paradox_form = paradox_form_render(win, max_y, max_x); // Render the paradox form

  // Refresh the window to show the paradox page
  wrefresh(win);
  pos_form_cursor(paradox_form); // Position the cursor for the current field

  int ch;
  while ((ch = wgetch(win)) != KEY_F(2)) {
    paradox_form_handle_input(win, ch);
  }

  paradox_form_destroy();

  curs_set(0);         // Hide the cursor
  nodelay(win, FALSE); // Make getch() blocking

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);
}