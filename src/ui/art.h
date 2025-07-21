#ifndef ART_H
#define ART_H

#include <ncurses/ncurses.h>
#include <string.h>

#include "../utils/utils.h"
#include "error.h"

extern const char* const BIRTHDAY_SIMULATION_ART_TEXT[];
extern const unsigned short BIRTHDAY_SIMULATION_ART_TEXT_LINE;

void art_text_center_render(WINDOW* win, int max_y, int max_x);

#endif