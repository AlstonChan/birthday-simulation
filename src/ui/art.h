#ifndef ART_H
#define ART_H

#include <ncurses/ncurses.h>
#include <string.h>

#include "../utils/utils.h"
#include "error.h"

extern const char* const BIRTHDAY_SIMULATION_ART_TEXT[];
extern const unsigned short BIRTHDAY_SIMULATION_ART_TEXT_LINE;

/**
 * @brief Print the birthday simulation art text in the center of the window.
 *
 * @param win The window to render the content at. Defaults to stdscr if NULL
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 * @return void
 */
void art_text_center_render(WINDOW* win, int max_y, int max_x);

#endif