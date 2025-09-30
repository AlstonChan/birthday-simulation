/**
 * \file            header.h
 * \brief           Header file for header.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HEADER_H
#define HEADER_H

#include <ncurses.h>
#include <string.h>

#include "../utils/utils.h"
#include "error.h"

void header_render(WINDOW* header_win);

#endif