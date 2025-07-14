#ifndef RESIZE_H
#define RESIZE_H

#include <windows.h>
#include <ncurses/ncurses.h>
#include <stdbool.h>

/**
 * @brief Check if the console window has been resized.
 * See this 
 * [stackoverflow post](https://stackoverflow.com/questions/78082999/check-for-terminal-resizing-using-ncurses-under-windows) 
 * for the source of this code:
 *
 * @param info COORD to return new window size
 * @return TRUE if the console window has changed size.  FALSE if not.
 */
bool check_console_window_resize_event(COORD *info);

#endif 