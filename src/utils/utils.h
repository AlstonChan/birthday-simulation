/**
 * \file            utils.h
 * \brief           Header file for utils.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef UTILS_H
#define UTILS_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#include <limits.h>
#include <ncurses/ncurses.h>
#include <openssl/rand.h>
#include <stdlib.h>
#include <string.h>

#include "../ui/error.h"

#define BH_MAIN_COLOR_PAIR      1
#define BH_ERROR_COLOR_PAIR     2
#define BH_SUCCESS_COLOR_PAIR   3
#define BH_WARNING_COLOR_PAIR   4
#define BH_INFO_COLOR_PAIR      5
#define BH_HIGHLIGHT_COLOR_PAIR 6

void print_in_middle(WINDOW* win, unsigned int start_y, unsigned int start_x, unsigned int width,
                     const char* string, chtype color);

size_t generate_random_input(uint8_t* buffer, size_t min_len, size_t max_len);

char* bytes_to_hex(const uint8_t* data, size_t len, bool uppercase);

uint8_t init_color_pairs();

bool binary_search(unsigned short arr[], unsigned short size, unsigned short target);

bool is_prime(unsigned int n);

unsigned int next_prime(unsigned int n);

void error_exit();

#endif