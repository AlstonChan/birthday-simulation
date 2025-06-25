#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "../art.h"
#include "../menu.h"
#include "main_menu.h"

/**
 * @brief The main menu choices for the program.
 *
 */
const struct ListMenuItem main_menu_choices[] = {
    {"Birthday Paradox Simulation", NULL},
    {"Attack Simulation", NULL},
    {"Explanation", NULL},
    {"System Info", NULL},
    {"Exit", NULL},
};
const unsigned short main_menu_choices_len = ARRAY_SIZE(main_menu_choices);

static ITEM **main_menu_choices_items = NULL;
static MENU *main_menu = NULL;
static WINDOW *main_menu_sub_win = NULL;

MENU *main_menu_get() { return main_menu; }

void main_menu_init(WINDOW *win) {
  list_menu_init(win,
                 main_menu_choices,
                 main_menu_choices_len,
                 &main_menu_choices_items,
                 &main_menu,
                 &main_menu_sub_win);
}

MENU *main_menu_render(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  if (!main_menu)
    main_menu_init(win); // Initialize the menu if not already done

  // Resize the window for the menu
  unsigned short main_menu_win_rows = ARRAY_SIZE(main_menu_choices) + 4;
  wresize(win, main_menu_win_rows, 40);

  // Center the menu window
  int y = (max_y - 15) / 2;
  int x = (max_x - 40) / 2;
  mvwin(win, y, x);

  box(win, 0, 0);
  print_in_middle(win, 0, 0, 40, " Main Menu ", COLOR_PAIR(BH_MAIN_COLOR_PAIR));

  art_text_center_render(NULL);

  // Render the menu navigation text
  list_menu_navigation_render(NULL, y + main_menu_win_rows + 1, -1, false);

  wrefresh(win);

  // If the menu window is not the standard screen, refresh it
  if (win != stdscr)
    refresh();

  return main_menu;
}

void main_menu_erase() {
  if (!main_menu)
    return;

  unpost_menu(main_menu); // Erase the menu from the window
}

void main_menu_restore(WINDOW *win, int max_y, int max_x) {
  if (!main_menu)
    return;

  post_menu(main_menu);                // Post the menu to the window
  main_menu_render(win, max_y, max_x); // Render the menu in the window
  wrefresh(main_menu_sub_win);
}

void main_menu_destroy() {
  if (!main_menu)
    return;

  main_menu_erase();    // Erase the menu from the window
  free_menu(main_menu); // Free the memory allocated for the menu

  for (unsigned short i = 0; i < ARRAY_SIZE(main_menu_choices); ++i)
    free_item(main_menu_choices_items[i]); // Free the memory allocated for each item
  free(main_menu_choices_items[ARRAY_SIZE(main_menu_choices)]); // Free the NULL terminator

  main_menu = NULL;               // Set the menu pointer to NULL
  main_menu_choices_items = NULL; // Set the items pointer to NULL
}