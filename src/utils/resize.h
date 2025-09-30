/**
 * \file            resize.h
 * \brief           Header file for resize.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RESIZE_H
#define RESIZE_H

#include <ncurses.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef COORD
/**
         * \brief           Placeholder for Windows COORD structure on POSIX
         */
typedef struct _COORD {
    short X;
    short Y;
} COORD, *PCOORD;
#endif
#endif

bool check_console_window_resize_event(COORD* info);

#endif