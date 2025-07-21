#ifndef ATTACK_H
#define ATTACK_H

#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <string.h>
#include <windows.h>

#include "../ui/attack/hash_collision.h"
#include "../ui/attack/hash_config.h"
#include "../ui/attack/hash_menu.h"
#include "../ui/error.h"
#include "../ui/footer.h"
#include "../ui/header.h"
#include "../utils/resize.h"

/**
 * @brief Renders the birthday attack page in the given window.
 *
 * @param content_win The window to render the attack page on
 * @param header_win The window to render the header content, normally for
 * the args of header_render
 * @param footer_win The window to render the footer content, normally for
 * the args of footer_render
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 */
void render_attack_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int max_y,
                        int max_x);

#endif
