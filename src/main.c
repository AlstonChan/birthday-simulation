#include <locale.h>
#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "ui/footer.h"
#include "ui/layout.h"
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

  WINDOW *header_win = newwin(2, max_x, 0, 0);
  WINDOW *footer_win = newwin(2, max_x, max_y - 2, 0);
  WINDOW *content_win = newwin(max_y - 4, max_x, 2, 0);
  keypad(content_win, TRUE); // Enable special keys in the menu window

  main_menu_init(content_win); // Initialize the menu in the content window
  MENU *main_menu = main_menu_render(content_win, max_y, max_x); // Render the menu

  footer_render(footer_win); // Render the footer

  bool is_done = false;
  while ((c = getch()) != KEY_F(1) && !is_done) {
    int selected_item = item_index(current_item(main_menu)); // Get the selected item index

    switch (c) {
    case KEY_DOWN:
    case '\t': // Tab key
      // If the user presses down on the last item, wrap around to the first item
      if (selected_item == main_menu_choices_len - 1) {
        menu_driver(main_menu, REQ_FIRST_ITEM);
      } else {
        menu_driver(main_menu, REQ_DOWN_ITEM);
      }
      break;
    case KEY_UP:
    case KEY_BTAB: // Shift + Tab key
      // If the user presses up on the first item, wrap around to the last item
      if (selected_item == 0) {
        menu_driver(main_menu, REQ_LAST_ITEM);
      } else {
        menu_driver(main_menu, REQ_UP_ITEM);
      }
      break;
    case KEY_ENTER:
    case 10: // Enter key
      switch (selected_item) {
      case 0:
        // Call the birthday paradox simulation function
        page_layout_render(header_win,
                           footer_win,
                           content_win,
                           max_y,
                           max_x,
                           PARADOX_WIN); // Render the paradox page
        break;
      case 1:
        // Call the birthday attack demo function
        // birthday_attack_demo();
        break;
      case 2:
        // Call the explanation function
        // explanation();
        break;
      case 3:
        // Call the system info function
        page_layout_render(header_win,
                           footer_win,
                           content_win,
                           max_y,
                           max_x,
                           SYSTEM_INFO_WIN); // Render system info
        break;
      case 4:
        // Exit the program
        is_done = true;
        break;
      default:
        // This case should never be reached, exit the loop
        is_done = true;
        break;
      }
      break;
    }
    wrefresh(content_win); // Refresh the menu window to show the current selection
  }

  main_menu_destroy(); // Destroy the menu

  endwin(); // End ncurses mode
  return 0;
}
