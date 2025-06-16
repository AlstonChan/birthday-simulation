#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <string.h>

#include "../ui/attack/hash_collision.h"
#include "../ui/attack/hash_config.h"
#include "../ui/attack/hash_menu.h"

/**
 * @brief The title of the attack page.
 *
 */
static const char *attack_page_title = "[ Birthday Attack Demo ]";

void render_attack_page(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr;

  nodelay(win, TRUE); // Make getch() non-blocking

  // Clear the window before rendering
  werase(win);

  hash_menu_restore(win, max_y, max_x); // Restore the menu to the content window if it was erased

  bool hash_menu_init_status = hash_menu_init(win); // Initialize the hash menu
  MENU *hash_menu = hash_menu_init_status ? hash_menu_render(win, max_y, max_x)
                                          : hash_menu_get(); // Render the hash menu

  unsigned short title_len = strlen(attack_page_title);
  mvwprintw(win, 0, (max_x - title_len) / 2, attack_page_title);

  bool is_done = false;
  int char_input;
  while ((char_input = wgetch(win)) != KEY_F(2) && !is_done) {
    int selected_item = item_index(current_item(hash_menu)); // Get the selected item index

    switch (char_input) {
    case KEY_DOWN:
    case '\t': // Tab key
      // If the user presses down on the last item, wrap around to the first item
      if (selected_item == hash_config_len - 1) {
        menu_driver(hash_menu, REQ_FIRST_ITEM);
      } else {
        menu_driver(hash_menu, REQ_DOWN_ITEM);
      }
      break;
    case KEY_UP:
    case KEY_BTAB: // Shift + Tab key
      // If the user presses up on the first item, wrap around to the last item
      if (selected_item == 0) {
        menu_driver(hash_menu, REQ_LAST_ITEM);
      } else {
        menu_driver(hash_menu, REQ_UP_ITEM);
      }
      break;
    case KEY_ENTER:
    case 10: // Enter key
      switch (selected_item) {
      case 0: // 8-bit hash
        hash_menu_erase();
        render_hash_collision_page(win, max_y, max_x, HASH_CONFIG_8BIT);
        break;
      }

      // Back to menu after exiting the hash collision page
      hash_menu_restore(win, max_y, max_x);
    }
  }

  hash_menu_destroy();

  nodelay(win, FALSE); // Make getch() blocking

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);

  erase();
}
