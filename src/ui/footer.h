#ifndef FOOTER_H
#define FOOTER_H

#include <ncurses/ncurses.h>
#include <string.h>

/**
 * @brief Render the footer of the application
 * 
 * @param footer_win The window to render the content at.
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 */
void footer_render(WINDOW *footer_win, int max_y, int max_x);

#endif