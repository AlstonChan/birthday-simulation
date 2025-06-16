#include <ncurses/ncurses.h>
#include <stdint.h>
#include <string.h>

#include "hash_collision.h"
#include "hash_config.h"

static const char *hash_collision_page_title = "[ Hash Collision Demonstration ]";

void render_hash_collision_page(WINDOW *win, int max_y, int max_x, enum hash_function_ids hash_id) {
  curs_set(1);        // Show the cursor
  nodelay(win, TRUE); // Make getch() non-blocking

  // Clear the window before rendering
  werase(win);

  wresize(win, max_y - 6, max_x);
  mvwin(win, 4, 0);
  box(win, 0, 0);

  unsigned short title_len = strlen(hash_collision_page_title);
  mvwprintw(win, 0, (max_x - title_len) / 2, hash_collision_page_title);

  // Refresh the window to show the changes
  wrefresh(win);

  bool is_done = false;
  int char_input;
  while ((char_input = wgetch(win)) != KEY_F(2) && !is_done) {
  }

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);

  curs_set(0);         // Hide the cursor
  nodelay(win, FALSE); // Make getch() blocking
}