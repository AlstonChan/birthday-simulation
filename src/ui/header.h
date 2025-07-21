#ifndef HEADER_H
#define HEADER_H

#include <ncurses/ncurses.h>
#include <string.h>

#include "../utils/utils.h"
#include "error.h"

void header_render(WINDOW* header_win);

#endif