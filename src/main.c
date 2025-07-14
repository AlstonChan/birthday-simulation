#include <locale.h>
#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "ui/error.h"
#include "ui/footer.h"
#include "ui/home/main_menu.h"
#include "ui/layout.h"
#include "utils/resize.h"
#include "utils/utils.h"

int main() {
  setlocale(LC_ALL, ""); // Set the locale to the user's default (utf-8)

  initscr(); // Initialize ncurses
  if (init_color_pairs() != 0) {
    render_full_page_error(stdscr, 0, 0, "Your terminal does not supports colours");
    return 1;
  }

  cbreak();              // Disable line buffering
  noecho();              // Don't echo user input
  keypad(stdscr, TRUE);  // Enable special keys (like arrow keys)
  nodelay(stdscr, TRUE); // Make getch() non-blocking
  curs_set(0);           // Hide the cursor

  COORD win_size;

  int max_y, max_x;               // Store the stdscr dimensions
  getmaxyx(stdscr, max_y, max_x); // Get initial window size

  WINDOW *header_win = newwin(2, max_x, 0, 0);
  WINDOW *footer_win = newwin(2, max_x, max_y - 2, 0);
  WINDOW *content_win = newwin(max_y - 4, max_x, 2, 0);
  keypad(content_win, TRUE); // Enable special keys in the menu window

  main_menu_init(content_win);
  MENU *main_menu = main_menu_render(content_win, max_y, max_x);

  footer_render(footer_win, 0, 0);

  int char_input;
  bool is_done = false;
  while ((char_input = getch()) != KEY_F(1) && !is_done) {
    int selected_item = item_index(current_item(main_menu));

    switch (char_input) {
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
        page_layout_render(header_win, footer_win, content_win, max_y, max_x, PARADOX_WIN);
        break;
      case 1:
        page_layout_render(header_win, footer_win, content_win, max_y, max_x, ATTACK_WIN);
        break;
      case 2:
        page_layout_render(header_win, footer_win, content_win, max_y, max_x, EXPLANATION_WIN);
        break;
      case 3:
        page_layout_render(header_win, footer_win, content_win, max_y, max_x, SYSTEM_INFO_WIN);
        break;
      case 4:
        is_done = true;
        break;
      default:
        // This case should never be reached, exit the loop
        is_done = true;
        break;
      }
      break;
    }

    // Check if terminal was resized
    if (check_console_window_resize_event(&win_size)) {
      int resize_result = resize_term(win_size.Y, win_size.X);
      if (resize_result != OK) {
        render_full_page_error(
            stdscr, 0, 0, "Unable to resize the UI to the terminal new size. Resize failure.");
      }

      max_y = win_size.Y;
      max_x = win_size.X;

      clear();
      wclear(footer_win);

      main_menu_restore(content_win, win_size.Y, win_size.X);
      footer_render(footer_win, win_size.Y, win_size.X);

      refresh();
    } else {
      wrefresh(content_win);
    }
  }

  main_menu_destroy();
  endwin(); // End ncurses mode
  return 0;
}
