#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <string.h>
/**
 * @brief The title of the attack page.
 *
 */
static const char *attack_page_title = "[ Birthday Attack Demo ]";

void render_attack_page(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr;

  curs_set(1);        // Show the cursor
  nodelay(win, TRUE); // Make getch() non-blocking

  // Clear the window before rendering
  werase(win);

  // Resize the window for the paradox page
  wresize(win, max_y - 6, max_x);

  // Center the paradox page window
  mvwin(win, 4, 0);

  box(win, 0, 0);

  unsigned short title_len = strlen(attack_page_title);
  mvwprintw(win, 0, (max_x - title_len) / 2, attack_page_title);

  int ch;
  while ((ch = wgetch(win)) != KEY_F(2)) {
    // TODO: Handle input
  }

  curs_set(0);         // Hide the cursor
  nodelay(win, FALSE); // Make getch() blocking

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);
}
