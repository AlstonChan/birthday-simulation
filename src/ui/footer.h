#ifndef FOOTER_H
#define FOOTER_H

#include <ncurses/ncurses.h>
#include <string.h>

#include "error.h"

void footer_render(WINDOW* footer_win, int max_y, int max_x);

#endif