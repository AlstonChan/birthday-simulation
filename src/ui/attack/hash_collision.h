/**
 * \file            hash_collision.h
 * \brief           Header file for hash_collision.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HASH_COLLISION_H
#define HASH_COLLISION_H

#include <ctype.h>
#include <glib.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "hash_config.h"

#include "../../utils/hash_function.h"
#include "../../utils/resize.h"
#include "../../utils/utils.h"
#include "../error.h"
#include "../footer.h"
#include "../form.h"
#include "../header.h"
#include "../layout.h"

void render_hash_collision_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win,
                                int* max_y, int* max_x, enum hash_function_ids hash_id,
                                GThreadPool* thread_pool);

#endif