#ifndef RESIZE_H
#define RESIZE_H

#include <ncurses/ncurses.h>
#include <stdbool.h>
#include <windows.h>

bool check_console_window_resize_event(COORD* info);

#endif