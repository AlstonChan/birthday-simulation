#ifndef PARADOX_H
#define PARADOX_H

#include <ncurses/ncurses.h>
#include <ncurses/form.h>

 

void render_paradox_page(WINDOW *win, int max_y, int max_x);
void paradox_form_init(WINDOW *win);
void paradox_form_destroy();
FORM *paradox_form_render(WINDOW *win, int max_y, int max_x);

#endif 