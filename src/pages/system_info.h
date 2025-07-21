#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <ncurses/ncurses.h>
#include <openssl/crypto.h>
#include <windows.h>

#include "../ui/error.h"
#include "../ui/footer.h"
#include "../ui/header.h"
#include "../utils/resize.h"
#include "version.h"

/**
 * @brief Render the system information page given a window.
 *
 * @param content_win The window to render the system information in.
 * @param header_win The window to render the header content, normally for
 * the args of header_render
 * @param footer_win The window to render the footer content, normally for
 * the args of footer_render
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 */
void render_system_info(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int max_y,
                        int max_x);

#endif