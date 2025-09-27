/**
 * \file            system_info.h
 * \brief           Header file for system_info.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <ncurses.h>
#include <openssl/crypto.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../ui/error.h"
#include "../ui/footer.h"
#include "../ui/header.h"
#include "../utils/resize.h"
#include "version.h"

void render_system_info(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int* max_y,
                        int* max_x);

#endif