/**
 * \file            layout.h
 * \brief           Header file for layout.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LAYOUT_H
#define LAYOUT_H

#include <ncurses.h>
#include <string.h>

#include "../pages/attack.h"
#include "../pages/explanation.h"
#include "../pages/paradox.h"
#include "../pages/system_info.h"

#include "error.h"
#include "footer.h"
#include "header.h"
#include "home/main_menu.h"

#define BH_LAYOUT_PADDING 6

/**
 * \brief          The type of page that the layout can render.
 *
 */
typedef enum {
    PARADOX_WIN = 0,
    ATTACK_WIN = 1,
    EXPLANATION_WIN = 2,
    SYSTEM_INFO_WIN = 3,
} PageType;

void page_layout_render(WINDOW* header_win, WINDOW* footer_win, WINDOW* content_win, int* max_y,
                        int* max_x, PageType page_type);

#endif