#ifndef PARADOX_H
#define PARADOX_H

#include <ctype.h>
#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "../ui/error.h"
#include "../ui/footer.h"
#include "../ui/header.h"
#include "../ui/paradox/paradox_form.h"
#include "../utils/resize.h"
#include "../utils/utils.h"

#define CONTENT_WIN_PADDING 6

/**
 * @brief Renders the paradox page in the given window.
 * If no window is provided, it will use stdscr.
 *
 * @param content_win The window to render the paradox page on
 * @param header_win The window to render the header content, normally for 
 * the args of header_render
 * @param footer_win The window to render the footer content, normally for 
 * the args of footer_render
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 */
void render_paradox_page(WINDOW *content_win, WINDOW *header_win, WINDOW *footer_win, int max_y, int max_x);
 
#endif 