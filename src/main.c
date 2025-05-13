#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void print_in_middle(WINDOW *win, int start_y, int start_x, int width, const char *string,
                     chtype color);

// Menu items
const char *const main_menu_choices[] = {
    "Birthday Paradox Simulation", "Birthday Attack Demo", "Explanation", "System Info", "Exit"};
const unsigned short main_menu_choices_len = ARRAY_SIZE(main_menu_choices);

int current_choice = 0;
int prev_max_y = 0; // Store previous dimensions
int prev_max_x = 0;

int main() {
  // Allocate memory for the menu items pointer array
  ITEM **main_menu_choices_items =
      (ITEM **)calloc((size_t)(main_menu_choices_len + 1), sizeof(ITEM *));
  MENU *main_menu;
  int c;

  initscr();             // Initialize ncurses
  start_color();         // Start color functionality
  cbreak();              // Disable line buffering
  noecho();              // Don't echo user input
  keypad(stdscr, TRUE);  // Enable special keys (like arrow keys)
  nodelay(stdscr, TRUE); // Make getch() non-blocking

  int max_y, max_x;               // Store the stdscr dimensions
  getmaxyx(stdscr, max_y, max_x); // Get initial window size
  prev_max_y = max_y;
  prev_max_x = max_x;

  WINDOW *menu_win =
      newwin(max_y / 2, max_x / 2, max_y / 4, max_x / 4); // Create a new window for the menu
  keypad(menu_win, TRUE);                                 // Enable special keys in the menu window
  box(menu_win, 0, 0);                                    // Draw a box around the window
  print_in_middle(menu_win, 1, 0, 40, "My Menu", COLOR_PAIR(1));
  refresh();

  // Create menu items for each choice
  for (unsigned short i = 0; i < main_menu_choices_len; i++) {
    main_menu_choices_items[i] = new_item(main_menu_choices[i], NULL);
  }
  main_menu_choices_items[main_menu_choices_len] =
      NULL; // Add NULL terminator to the end of the array

  main_menu = new_menu(main_menu_choices_items); // Create the menu
  set_menu_win(main_menu, menu_win);
  set_menu_sub(main_menu, derwin(menu_win, 6, 38, 3, 1));
  mvprintw(LINES - 2, 0, "F1 to Exit");
  post_menu(main_menu); // Post the menu to the window
  wrefresh(menu_win);   // Refresh the menu window to show the menu

  while ((c = getch()) != KEY_F(1)) {
    switch (c) {
    case KEY_DOWN:
      menu_driver(main_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(main_menu, REQ_UP_ITEM);
      break;
    }
    wrefresh(menu_win); // Refresh the menu window to show the current selection
  }

  /* Unpost and free all the memory taken up */
  unpost_menu(main_menu);
  free_menu(main_menu);
  for (unsigned short i = 0; i < main_menu_choices_len; ++i)
    free_item(main_menu_choices_items[i]);

  endwin(); // End ncurses mode
  return 0;
}

void print_in_middle(WINDOW *win, int start_y, int start_x, int width, const char *string,
                     chtype color) {
  int length, x, y;
  float temp;

  if (win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if (start_x != 0)
    x = start_x;
  if (start_y != 0)
    y = start_y;
  if (width == 0)
    width = 80;

  length = (int)strlen(string);
  temp = (float)(width - length) / 2;
  x = start_x + (int)temp;
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
  refresh();
}