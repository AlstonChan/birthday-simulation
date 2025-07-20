#ifndef LAYOUT_H
#define LAYOUT_H

#include <ncurses/ncurses.h>
#include <string.h>

#include "../pages/attack.h"
#include "../pages/explanation.h"
#include "../pages/paradox.h"
#include "../pages/system_info.h"

#include "footer.h"
#include "header.h"
#include "error.h"
#include "home/main_menu.h"

#define BH_LAYOUT_PADDING 6

/**
 * @brief The type of page that the layout can render.
 * 
 */
typedef enum {
  PARADOX_WIN = 0,
  ATTACK_WIN = 1,
  EXPLANATION_WIN = 2,
  SYSTEM_INFO_WIN = 3,
} PageType;

/**
 * @brief Renders the paradox page in the given window.
 * If no window is provided, it will use stdscr.
 *
 * @param header_win The window to render the header content, normally for 
 * the args of header_render
 * @param footer_win The window to render the footer content, normally for 
 * the args of footer_render
 * @param content_win The window to render the main content on
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 * @param page_type The type of page to render the content
 */
void page_layout_render(WINDOW *header_win, WINDOW *footer_win, 
                           WINDOW *content_win, int max_y, int max_x, PageType page_type);

#endif 