/**
 * \file            art.h
 * \brief           Header file for art.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef ART_H
#define ART_H

#include <ncurses.h>
#include <string.h>

#include "../utils/utils.h"
#include "error.h"

extern const char* const BIRTHDAY_SIMULATION_ART_TEXT[];
extern const unsigned short BIRTHDAY_SIMULATION_ART_TEXT_LINE;

void art_text_center_render(WINDOW* win, int max_y, int max_x);

#endif