/**
 * \file            hash_menu.h
 * \brief           Header file for hash_menu.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HASH_MENU_H
#define HASH_MENU_H

#include <ncurses/menu.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "../menu.h"
#include "hash_config.h"

MENU* hash_menu_get();

bool hash_menu_init(WINDOW* win);

MENU* hash_menu_render(WINDOW* win, int max_y, int max_x);

void hash_menu_erase();

void hash_menu_restore(WINDOW* win, int max_y, int max_x);

void hash_menu_destroy();

#endif
