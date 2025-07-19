#ifndef PARADOX_H
#define PARADOX_H

#include <ncurses/ncurses.h>
#include <ncurses/form.h>

void render_paradox_page(WINDOW *win, WINDOW *header_win, WINDOW *footer_win,int max_y, int max_x);
 
#endif 