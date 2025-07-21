/**
 * \file            paradox_math.h
 * \brief           Header file for paradox_math.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef PARADOX_MATH_H
#define PARADOX_MATH_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

double calculate_birthday_collision_probability(int domain_size, int sample_size);

double simulate_birthday_collision(int domain_size, int sample_size, int num_runs);

#endif
