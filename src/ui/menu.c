#include "menu.h"

void list_menu_init(WINDOW *win, const struct ListMenuItem choices[], unsigned short choices_len,
                    ITEM ***choices_items, MENU **menu, WINDOW **sub_win) {
  if (win == NULL) {
    render_full_page_error_exit(stdscr, 0, 0, "The window passed to list_menu_init is null");
  }

  // Allocate memory for the menu items pointer array
  *choices_items = (ITEM **)calloc((size_t)(choices_len + 1), sizeof(ITEM *));

  // Create menu items for each choice
  for (unsigned short i = 0; i < choices_len; ++i) {
    (*choices_items)[i] = new_item(choices[i].label, choices[i].description);
  }

  // Add NULL terminator to the end of the array
  (*choices_items)[choices_len] = NULL;

  // Create a sub-window for the menu
  if (*sub_win == NULL) {
    *sub_win = derwin(win, 6, 38, 2, 1);
  }

  *menu = new_menu(*choices_items); // Create the menu
  set_menu_win(*menu, win);         // Set the window for the menu
  set_menu_sub(*menu, *sub_win);    // Create a sub-window for the menu
  set_menu_mark(*menu, "> ");       // Set the mark for selected items
  post_menu(*menu);                 // Post the menu to the window
}

void list_menu_navigation_render(WINDOW *win, int y, int x, bool hide_exit_text) {
  if (win == NULL) {
    render_full_page_error_exit(
        stdscr, 0, 0, "The window passed to list_menu_navigation_render is null");
  }

  const char *menu_navigation_text = "[↑/↓]: Navigate   [Enter]: Select   [F1]: Exit";
  if (hide_exit_text) {
    // If the exit text should be hidden, remove it from the navigation text
    menu_navigation_text = "[↑/↓]: Navigate   [Enter]: Select";
  }

  const unsigned short menu_navigation_text_len = strlen(menu_navigation_text);

  if (x < 0)
    x = (COLS - menu_navigation_text_len) / 2; // Center the text if x is negative

  mvwprintw(win, y, x, "%s", menu_navigation_text);
}