/**
 * \file            footer.h
 * \brief           Header file for footer.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef FOOTER_H
#define FOOTER_H

#include <ncurses.h>
#include <string.h>

#include "error.h"

void footer_render(WINDOW* footer_win, int max_y, int max_x);

#endif