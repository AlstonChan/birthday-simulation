/**
 * \file            resize.c
 * \brief           The core logic behind detecting the terminal size and notify
 *                  the application via the always updated COORD. So that the 
 *                  application can notify ncurses to resize and perform required
 *                  screen clearing and repainting for each resize.
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "resize.h"

static HANDLE s_handle_console_output = NULL;

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Get the console output handle object.
 *                 See [Console Handles
 *                 documentation](https://learn.microsoft.com/en-us/windows/console/console-handles)
 *                 for more details.
 *
 * \return         The actual console handle of the terminal
 */
static HANDLE
get_console_output_handle(void) {
    SECURITY_ATTRIBUTES security_attributes;
    if (!s_handle_console_output) {
        // First call - get the window handle once and save it
        security_attributes.nLength = sizeof(security_attributes);
        security_attributes.lpSecurityDescriptor = NULL;
        security_attributes.bInheritHandle = TRUE;

        // Using CreateFile we got the true console handle, avoiding any redirection
        s_handle_console_output = CreateFile(
            TEXT("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            &security_attributes, OPEN_EXISTING, (DWORD)0, (HANDLE)0);
    }
    return s_handle_console_output;
}

/****************************************************************
                       EXTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Check if the console window has been resized.
 *                 See this
 *                 [stackoverflow
 *                 post](https://stackoverflow.com/questions/78082999/check-for-terminal-resizing-using-ncurses-under-windows)
 *                 for the source of this code:
 *
 * \param[out]     info COORD to return new window size
 * \return         TRUE if the console window has changed size. FALSE if not.
 */
bool
check_console_window_resize_event(COORD* info) {
    // We hold the old screen size between calls to this function
    static short old_screen_w = 0, old_screen_h = 0;
    short current_screen_w, current_screen_h;
    CONSOLE_SCREEN_BUFFER_INFO console_scr_buf_info;

    s_handle_console_output = get_console_output_handle();
    if (!s_handle_console_output) {
        return false;
    }

    if (!GetConsoleScreenBufferInfo(s_handle_console_output, &console_scr_buf_info)) {
        return false;
    }

    current_screen_w = console_scr_buf_info.srWindow.Right - console_scr_buf_info.srWindow.Left + 1;
    current_screen_h = console_scr_buf_info.srWindow.Bottom - console_scr_buf_info.srWindow.Top + 1;

    if (!old_screen_w && !old_screen_h) {
        /* Execution comes here if this is first time this function is called.
    ** Initialize the static variables and bail...*/
        old_screen_w = current_screen_w;
        old_screen_h = current_screen_h;
        return false;
    }

    // At last the real work of this function can be realized...
    if (current_screen_w != old_screen_w || current_screen_h != old_screen_h) {
        old_screen_w = current_screen_w;
        old_screen_h = current_screen_h;
        info->X = current_screen_w;
        info->Y = current_screen_h;
        return true;
    }
    return false;
}