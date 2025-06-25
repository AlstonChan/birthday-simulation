#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "../menu.h"
#include "hash_config.h"
#include "hash_menu.h"

static ITEM **hash_menu_choices_items = NULL;
static MENU *hash_menu = NULL;
static WINDOW *hash_menu_sub_win = NULL;

MENU *hash_menu_get() {
  if (!hash_menu) {
    // If the menu is not initialized, exit the program
    fprintf(stderr, "Hash menu is not initialized. Call hash_menu_init() first.\n");
    exit(EXIT_FAILURE);
  }

  return hash_menu; // Return the current hash menu
}

static int hash_menu_window_cols() { return MENU_PADDING_Y + hash_config_len + MENU_PADDING_Y; }
const int hash_menu_window_rows = 40; ///< The number of rows for the hash menu window

bool hash_menu_init(WINDOW *win) {
  if (hash_menu)
    return false; // If the menu is already initialized, do nothing

  struct ListMenuItem *hash_menu_choices = get_hash_config_menu();

  // Resize the window for the menu BEFORE creating the sub-window
  // as the size of the sub-window depends on the main window size
  wresize(win, hash_menu_window_cols(), hash_menu_window_rows);

  hash_menu_sub_win = derwin(win, hash_config_len, 32, 2, 1); // Create a sub-window for the menu

  list_menu_init(win,
                 hash_menu_choices,
                 hash_config_len,
                 &hash_menu_choices_items,
                 &hash_menu,
                 &hash_menu_sub_win);
  return true;
}

MENU *hash_menu_render(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  if (!hash_menu)
    hash_menu_init(win); // Initialize the menu if not already done

  // Center the menu window
  int y = (max_y - 20) / 2;
  int x = (max_x - hash_menu_window_rows) / 2;
  mvwin(win, y, x);

  box(win, 0, 0);
  print_in_middle(
      win, 0, 0, hash_menu_window_rows, " Select hash function ", COLOR_PAIR(BH_MAIN_COLOR_PAIR));

  // Render the menu navigation text
  list_menu_navigation_render(NULL, y + hash_menu_window_cols() + 1, -1, true);

  wrefresh(win);

  // If the menu window is not the standard screen, refresh it
  if (win != stdscr)
    refresh();

  return hash_menu;
}

void hash_menu_erase() {
  if (!hash_menu)
    return;

  unpost_menu(hash_menu); // Erase the menu from the window
}

void hash_menu_restore(WINDOW *win, int max_y, int max_x) {
  if (!hash_menu)
    return;

  // Check if the window size is equal to hash_menu_window_cols(), resize it if not
  if (getmaxy(win) != hash_menu_window_cols() || getmaxx(win) != hash_menu_window_rows) {
    wresize(win, hash_menu_window_cols(), hash_menu_window_rows);

    // Resize the sub-window to match the new window size
    wresize(hash_menu_sub_win, hash_config_len, 32);
    mvwin(hash_menu_sub_win, 2, 1); // Move the sub-window to the correct position

    mvwin(win, (max_y - hash_menu_window_cols()) / 2, (max_x - hash_menu_window_rows) / 2);
  }

  post_menu(hash_menu);                // Post the menu to the window
  hash_menu_render(win, max_y, max_x); // Render the menu in the window
  wrefresh(hash_menu_sub_win);
  wrefresh(win);
}

void hash_menu_destroy() {
  if (!hash_menu)
    return;

  hash_menu_erase();    // Erase the menu from the window
  free_menu(hash_menu); // Free the memory allocated for the menu

  struct ListMenuItem *hash_menu_choices = get_hash_config_menu();

  for (unsigned short i = 0; i < ARRAY_SIZE(hash_menu_choices); ++i)
    free_item(hash_menu_choices_items[i]); // Free the memory allocated for each item
  free(hash_menu_choices_items[ARRAY_SIZE(hash_menu_choices)]); // Free the NULL terminator

  hash_menu = NULL;               // Set the menu pointer to NULL
  hash_menu_choices_items = NULL; // Set the items pointer to NULL
}