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

#include <ctype.h>
#include <libgen.h>
#include <math.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../ui/error.h"
#include "../ui/footer.h"
#include "../ui/header.h"
#include "../ui/layout.h"
#include "../utils/resize.h"
#include "../utils/utils.h"
#include "explanation_embedded.h"

void render_explanation_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win,
                             int* max_y, int* max_x);

#endif
