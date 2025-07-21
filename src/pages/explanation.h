/**
 * \file            explanation.h
 * \brief           Header file for explanation.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EXPLANATION_H
#define EXPLANATION_H

#include <ncurses/form.h>
#include <ncurses/ncurses.h>

void render_explanation_page(WINDOW* win, int max_y, int max_x);

#endif
