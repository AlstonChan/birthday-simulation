#ifndef ERROR_H
#define ERROR_H

#include <ncurses/ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/utils.h"

#define MAX_LINE_WIDTH 40

/**
 * @brief Render a full page error. This is suitable for error that are
 * no recoverable or requires a lot of attention.
 * 
 * @param win The window to render the content at. Defaults to stdscr if NULL
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 * @param message The message to show the user of the error
 */
void render_full_page_error(WINDOW *win, int max_y, int max_x, char *message);

/**
 * @brief Same as render_full_page_error function, but will exit after the 
 * function ends
 * 
 * @param win The window to render the content at. Defaults to stdscr if NULL
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 * @param message The message to show the user of the error
 * @param end 
 */
void render_full_page_error_exit(WINDOW *win, int max_y, int max_x, char *message);

#endif