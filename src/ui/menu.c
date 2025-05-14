#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "menu.h"

/**
 * @brief The main menu choices for the program.
 *
 */
const char *const main_menu_choices[] = {
    "Birthday Paradox Simulation", "Birthday Attack Demo", "Explanation", "System Info", "Exit"};

/**
 * @brief The number of choices in the main menu.
 *
 */
const unsigned short main_menu_choices_len = ARRAY_SIZE(main_menu_choices);

static ITEM **main_menu_choices_items = NULL;
static MENU *main_menu = NULL;

/**
 * @brief Initializes the main menu.
 *
 * @param win The window to display the menu in. If NULL, uses stdscr.
 */
void main_menu_init(WINDOW *win) {
  if (main_menu)
    return; // Already initialized

  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  // Allocate memory for the menu items pointer array
  main_menu_choices_items = (ITEM **)calloc((size_t)(main_menu_choices_len + 1), sizeof(ITEM *));

  // Create menu items for each choice
  for (unsigned short i = 0; i < main_menu_choices_len; ++i) {
    main_menu_choices_items[i] = new_item(main_menu_choices[i], NULL);
  }

  // Add NULL terminator to the end of the array
  main_menu_choices_items[main_menu_choices_len] = NULL;

  main_menu = new_menu(main_menu_choices_items);     // Create the menu
  set_menu_win(main_menu, win);                      // Set the window for the menu
  set_menu_sub(main_menu, derwin(win, 6, 38, 2, 1)); // Create a sub-window for the menu
  set_menu_mark(main_menu, "> ");                    // Set the mark for selected items
  post_menu(main_menu);                              // Post the menu to the window

  wrefresh(win); // Refresh the window to show the menu
}

/**
 * @brief Renders the menu in the specified window.
 *
 * @param win The window to render the menu in. If NULL, uses stdscr.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 * @return MENU* The rendered menu.
 */
MENU *main_menu_render(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  // Resize the window for the menu
  wresize(win, main_menu_choices_len + 4, 40);

  // Center the menu window
  mvwin(win, (max_y - 15) / 2, (max_x - 40) / 2);

  box(win, 0, 0);
  print_in_middle(win, 1, 0, 40, "Main Menu", COLOR_PAIR(1));
  wrefresh(win);

  return main_menu;
}

/**
 * @brief Gets the current menu.
 *
 * @return MENU* The current menu.
 */
MENU *main_menu_get() { return main_menu; }

/**
 * @brief Destroys the menu and frees allocated memory.
 *
 */
void main_menu_destroy() {
  if (!main_menu)
    return;

  unpost_menu(main_menu); // Erase the menu from the window
  free_menu(main_menu);   // Free the memory allocated for the menu

  for (unsigned short i = 0; i < main_menu_choices_len; ++i)
    free_item(main_menu_choices_items[i]);              // Free the memory allocated for each item
  free(main_menu_choices_items[main_menu_choices_len]); // Free the NULL terminator

  main_menu = NULL;               // Set the menu pointer to NULL
  main_menu_choices_items = NULL; // Set the items pointer to NULL
}