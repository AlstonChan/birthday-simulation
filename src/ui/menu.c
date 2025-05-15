#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "art.h"
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
static WINDOW *main_menu_sub_win = NULL;

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

  // Create a sub-window for the menu
  main_menu_sub_win = derwin(win, 6, 38, 2, 1);

  main_menu = new_menu(main_menu_choices_items); // Create the menu
  set_menu_win(main_menu, win);                  // Set the window for the menu
  set_menu_sub(main_menu, main_menu_sub_win);    // Create a sub-window for the menu
  set_menu_mark(main_menu, "> ");                // Set the mark for selected items
  post_menu(main_menu);                          // Post the menu to the window
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
  unsigned short main_menu_win_rows = main_menu_choices_len + 4;
  wresize(win, main_menu_win_rows, 40);

  // Center the menu window
  int y = (max_y - 15) / 2;
  int x = (max_x - 40) / 2;
  mvwin(win, y, x);

  box(win, 0, 0);
  print_in_middle(win, 0, 0, 40, " Main Menu ", COLOR_PAIR(1));

  art_text_center_render(NULL);

  // Render the menu navigation text
  main_menu_navigation_render(NULL, y + main_menu_win_rows + 1);

  wrefresh(win);

  // If the menu window is not the standard screen, refresh it
  if (win != stdscr)
    refresh();

  return main_menu;
}

void main_menu_navigation_render(WINDOW *win, int y) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  const char const *menu_navigation_text = "[↑/↓]: Navigate   [Enter]: Select   [F1]: Exit";
  const unsigned short menu_navigation_text_len = strlen(menu_navigation_text);

  mvwprintw(win, y, (COLS - menu_navigation_text_len) / 2, "%s", menu_navigation_text);
}

/**
 * @brief Gets the current menu.
 *
 * @return MENU* The current menu.
 */
MENU *main_menu_get() { return main_menu; }

/**
 * @brief Erases the menu from the window. So that the window can
 * be used for other purposes.
 *
 */
void main_menu_erase() {
  if (!main_menu)
    return;

  unpost_menu(main_menu); // Erase the menu from the window
}

/**
 * @brief Restores the menu to the window. This is useful
 * after the menu has been erased and you want to
 * display it again.
 *
 */
void main_menu_restore(WINDOW *win, int max_y, int max_x) {
  if (!main_menu)
    return;

  post_menu(main_menu);                // Post the menu to the window
  main_menu_render(win, max_y, max_x); // Render the menu in the window
  wrefresh(main_menu_sub_win);
}

/**
 * @brief Destroys the menu and frees allocated memory.
 *
 */
void main_menu_destroy() {
  if (!main_menu)
    return;

  main_menu_erase();    // Erase the menu from the window
  free_menu(main_menu); // Free the memory allocated for the menu

  for (unsigned short i = 0; i < main_menu_choices_len; ++i)
    free_item(main_menu_choices_items[i]);              // Free the memory allocated for each item
  free(main_menu_choices_items[main_menu_choices_len]); // Free the NULL terminator

  main_menu = NULL;               // Set the menu pointer to NULL
  main_menu_choices_items = NULL; // Set the items pointer to NULL
}