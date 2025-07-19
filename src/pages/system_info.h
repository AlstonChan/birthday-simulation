#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <ncurses/ncurses.h>
#include <ncurses/ncurses.h>
#include <openssl/crypto.h>

#include "version.h"
#include "../ui/error.h"

/**
 * @brief Render the system information page given a window.
 *
 * @param win The window to render the system information in.
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 */
void render_system_info(WINDOW *win, int max_y, int max_x);

#endif