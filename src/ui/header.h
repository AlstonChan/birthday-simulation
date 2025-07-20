#ifndef HEADER_H
#define HEADER_H

#include <ncurses/ncurses.h>
#include <string.h>

#include "../utils/utils.h"
#include "error.h"

/**
 * @brief Render the header of the application
 * 
 * @param header_win The window to render the content at.
 */
void header_render(WINDOW *header_win);

#endif