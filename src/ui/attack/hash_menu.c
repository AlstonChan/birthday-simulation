#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "../menu.h"
#include "hash_menu.h"

/**
 * @brief The choices for the hash menu.
 *
 */
const struct ListMenuItem hash_menu_choices[] = {
    {"Custom Hash", "(8-bit)"},
    {"Custom Hash", "(12-bit)"},
    {"Custom Hash", "(16-bit)"},
    {"RIPEMD-160", "(160-bit)"},
    {"SHA-1", "(160-bit)"},
    {"SHA3-256", "(256-bit)"},
    {"SHA-256", "(256-bit)"},
    {"SHA-512", "(512-bit)"},
    {"SHA-384", "(384-bit)"},
    {"Keccak-256", "(256-bit)"},
};
const unsigned short hash_menu_choices_len = ARRAY_SIZE(hash_menu_choices);

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

bool hash_menu_init(WINDOW *win) {
  if (hash_menu)
    return false; // If the menu is already initialized, do nothing

  // Resize the window for the menu BEFORE creating the sub-window
  // as the size of the sub-window depends on the main window size
  wresize(win, MENU_PADDING_Y + hash_menu_choices_len + MENU_PADDING_Y, 40);

  hash_menu_sub_win =
      derwin(win, hash_menu_choices_len, 32, 2, 1); // Create a sub-window for the menu

  list_menu_init(win,
                 hash_menu_choices,
                 hash_menu_choices_len,
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
  int x = (max_x - 40) / 2;
  mvwin(win, y, x);

  box(win, 0, 0);
  print_in_middle(win, 0, 0, 40, " Select hash function ", COLOR_PAIR(1));

  // Render the menu navigation text
  list_menu_navigation_render(
      NULL, y + MENU_PADDING_Y + hash_menu_choices_len + MENU_PADDING_Y + 1, -1, true);

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

  post_menu(hash_menu);                // Post the menu to the window
  hash_menu_render(win, max_y, max_x); // Render the menu in the window
  wrefresh(hash_menu_sub_win);
}

void hash_menu_destroy() {
  if (!hash_menu)
    return;

  hash_menu_erase();    // Erase the menu from the window
  free_menu(hash_menu); // Free the memory allocated for the menu

  for (unsigned short i = 0; i < ARRAY_SIZE(hash_menu_choices); ++i)
    free_item(hash_menu_choices_items[i]); // Free the memory allocated for each item
  free(hash_menu_choices_items[ARRAY_SIZE(hash_menu_choices)]); // Free the NULL terminator

  hash_menu = NULL;               // Set the menu pointer to NULL
  hash_menu_choices_items = NULL; // Set the items pointer to NULL
}