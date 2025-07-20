#include "layout.h"

void page_layout_render(WINDOW *header_win, WINDOW *footer_win, WINDOW *content_win, int max_y,
                        int max_x, PageType page_type) {
  // If either header, footer, or content window is NULL, return error
  if (header_win == NULL || footer_win == NULL || content_win == NULL) {
    render_full_page_error_exit(stdscr, 0, 0, "The window passed to page_layout_render is null");
  }

  // Erase the stdscr and main menu window
  erase();           // Clear the screen
  main_menu_erase(); // Erase the menu from the window
  refresh();         // Refresh the screen

  // If the stdscr has nodelay enabled, disable it
  bool nodelay_modified = false;
  if (is_nodelay(stdscr)) {
    nodelay(stdscr, FALSE);
    nodelay_modified = true; // Track if we modified nodelay
  }

  header_render(header_win);
  footer_render(footer_win, max_y, max_x);

  switch (page_type) {
  case PARADOX_WIN:
    render_paradox_page(
        content_win, header_win, footer_win, max_y, max_x); // Render the paradox page
    break;
  case ATTACK_WIN:
    render_attack_page(content_win, max_y, max_x); // Render the attack page
    break;
  case EXPLANATION_WIN:
    render_explanation_page(content_win, max_y, max_x); // Render the explanation page
    break;
  case SYSTEM_INFO_WIN:
    render_system_info(content_win,
                       header_win,
                       footer_win,
                       max_y,
                       max_x); // Render system info in the content window
    break;
  default:
    // If an unknown page type is provided, do nothing
    break;
  }

  // If we have modified nodelay, restore it
  if (nodelay_modified) {
    nodelay(stdscr, TRUE); // Restore nodelay to true
  }

  main_menu_restore(content_win, max_y, max_x); // Restore the menu to the content window

  werase(header_win);   // Clear the header window
  wrefresh(header_win); // Refresh the header window

  // Refresh the content window to show the current selection
  wrefresh(content_win);

  // Render the footer
  footer_render(footer_win, max_y, max_x);
}