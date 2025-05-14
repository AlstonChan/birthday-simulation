#include <locale.h>
#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "ui/art.h"
#include "ui/menu.h"
#include "utils/utils.h"

int main() {
  setlocale(LC_ALL, ""); // Set the locale to the user's default (utf-8)

  initscr(); // Initialize ncurses

  if (has_colors() == FALSE) { // Check if the terminal supports colors
    endwin();
    fprintf(stderr, "\nBirthday Simulation: Your terminal does not support color\n");
    return 1;
  }
  start_color(); // Start color functionality
  init_pair(1, COLOR_CYAN, COLOR_BLACK);

  cbreak();              // Disable line buffering
  noecho();              // Don't echo user input
  keypad(stdscr, TRUE);  // Enable special keys (like arrow keys)
  nodelay(stdscr, TRUE); // Make getch() non-blocking
  curs_set(0);           // Hide the cursor

  int c;

  int max_y, max_x;               // Store the stdscr dimensions
  getmaxyx(stdscr, max_y, max_x); // Get initial window size

  // WINDOW *header_win = newwin(2, max_x, 0, 0);
  // WINDOW *footer_win = newwin(2, max_x, max_y - 2, 0);
  WINDOW *content_win = newwin(max_y - 4, max_x, 2, 0);
  keypad(content_win, TRUE); // Enable special keys in the menu window

  main_menu_init(content_win); // Initialize the menu in the content window
  MENU *main_menu = main_menu_render(content_win, max_y, max_x); // Render the menu

  mvprintw(LINES - 2, 1, "F1 to Exit");
  ncurses_print_art_text_center(NULL); // Print the art text

  bool is_done = false;
  while ((c = getch()) != KEY_F(1) && !is_done) {
    switch (c) {
    case KEY_DOWN:
      // If the user presses down on the last item, wrap around to the first item
      if (item_index(current_item(main_menu)) == main_menu_choices_len - 1) {
        menu_driver(main_menu, REQ_FIRST_ITEM);
      } else {
        menu_driver(main_menu, REQ_DOWN_ITEM);
      }
      break;
    case KEY_UP:
      // If the user presses up on the first item, wrap around to the last item
      if (item_index(current_item(main_menu)) == 0) {
        menu_driver(main_menu, REQ_LAST_ITEM);
      } else {
        menu_driver(main_menu, REQ_UP_ITEM);
      }
      break;
    case KEY_ENTER:
    case 10: // Enter key
      if (item_index(current_item(main_menu)) == 0) {
        // Call the birthday paradox simulation function
        // birthday_paradox_simulation();
      } else if (item_index(current_item(main_menu)) == 1) {
        // Call the birthday attack demo function
        // birthday_attack_demo();
      } else if (item_index(current_item(main_menu)) == 2) {
        // Call the explanation function
        // explanation();
      } else if (item_index(current_item(main_menu)) == 3) {
        // Call the system info function
        // system_info();
      } else if (item_index(current_item(main_menu)) == 4) {
        // Exit the program
        is_done = true;
      }
      break;
    }
    wrefresh(content_win); // Refresh the menu window to show the current selection
  }

  main_menu_destroy(); // Destroy the menu

  endwin(); // End ncurses mode
  return 0;
}
