#ifndef LAYOUT_H
#define LAYOUT_H

#include <ncurses/ncurses.h>

/**
 * @brief The type of page that the layout can render.
 * 
 */
typedef enum {
  PARADOX_WIN,
  ATTACK_WIN,
  EXPLANATION_WIN,
  SYSTEM_INFO_WIN,
} PageType;

void page_layout_render(WINDOW *header_win, WINDOW *footer_win, 
                           WINDOW *content_win, int max_y, int max_x, PageType page_type);

#endif 