/**
 * \file            paradox.h
 * \brief           Header file for paradox.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

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

void render_paradox_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int max_y,
                         int max_x);

#endif