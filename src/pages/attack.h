/**
 * \file            attack.h
 * \brief           Header file for attack.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

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

void render_attack_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int* max_y,
                        int* max_x);

#endif
