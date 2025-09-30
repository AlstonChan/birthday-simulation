/**
 * \file            paradox_form.h
 * \brief           Header file for paradox_form.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef PARADOX_FORM_H
#define PARADOX_FORM_H

#include <ctype.h>
#include <form.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../ui/error.h"
#include "../../utils/paradox_math.h"
#include "../../utils/utils.h"
#include "../form.h"

void paradox_form_init(WINDOW* win, int max_y, int max_x);

FORM* paradox_form_render(WINDOW* win, int max_y, int max_x);

void paradox_form_destroy();

void paradox_form_handle_input(WINDOW* win, int ch, double* collision_probability,
                               double* simulated_runs_results);

void paradox_form_restore(WINDOW* win, int max_y, int max_x, double collision_probability,
                          double simulated_runs_results);

#endif