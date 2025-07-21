/**
 * \file            error.h
 * \brief           Header file for error.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef ERROR_H
#define ERROR_H

#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"

#define MAX_LINE_WIDTH 40

void render_full_page_error(WINDOW* win, int max_y, int max_x, char* message);

void render_full_page_error_exit(WINDOW* win, int max_y, int max_x, char* message);

#endif