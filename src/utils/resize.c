/**
 * \file            resize.c
 * \brief           Cross-platform core logic for detecting terminal size changes
 *                  and notifying the application via an always updated COORD structure.
 *                  This allows the application to notify ncurses to resize and perform
 *                  required screen clearing and repainting for each resize event.
 *                  
 *                  On Windows: Uses Windows Console API to detect size changes
 *                  On Linux: Uses SIGWINCH signal handler and ioctl() for terminal size
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "resize.h"

#ifdef _WIN32
/****************************************************************
                       WINDOWS IMPLEMENTATION
****************************************************************/

static HANDLE s_handle_console_output = NULL;

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

/**
 * \brief          Check if the console window has been resized.
 *                 See this
 *                 [stackoverflow
 *                 post](https://stackoverflow.com/questions/78082999/check-for-terminal-resizing-using-ncurses-under-windows)
 *                 for the source of this code:
 *
 * \param[out]     info COORD to return new window size
 * \return         true if the console window has changed size, false if not
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

#else
/****************************************************************
                       LINUX IMPLEMENTATION
****************************************************************/

/// Global flag set by SIGWINCH signal handler to indicate terminal resize
static volatile sig_atomic_t s_terminal_resized = 0;

/**
 * \brief          Signal handler for SIGWINCH (terminal resize signal)
 *                 This handler is called automatically when the terminal
 *                 window is resized by the user.
 * 
 * \param[in]      sig Signal number (should be SIGWINCH)
 * 
 * \note           This function only sets a flag and returns immediately
 *                 to ensure signal safety. The actual resize handling is
 *                 done in check_console_window_resize_event()
 */
static void
sigwinch_handler(int sig) {
    (void)sig; // Suppress unused parameter warning
    s_terminal_resized = 1;
}

/**
 * \brief          Get current terminal size using ioctl system call
 * 
 * \param[out]     width Pointer to store terminal width (columns)
 * \param[out]     height Pointer to store terminal height (rows)
 * \return         true if terminal size was successfully retrieved, false otherwise
 * 
 * \note           Uses ioctl(TIOCGWINSZ) to query the terminal size
 *                 Falls back to STDOUT_FILENO if STDIN_FILENO fails
 */
static bool
get_terminal_size(short* width, short* height) {
    struct winsize ws;

    // Try to get window size from stdin first
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == 0) {
        *width = (short)ws.ws_col;
        *height = (short)ws.ws_row;
        return true;
    }

    // Fallback to stdout
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        *width = (short)ws.ws_col;
        *height = (short)ws.ws_row;
        return true;
    }

    return false;
}

/**
 * \brief          Initialize signal handler for terminal resize detection
 *                 Sets up SIGWINCH signal handler to detect when the terminal
 *                 window is resized by the user.
 * 
 * \return         true if signal handler was successfully installed, false otherwise
 * 
 * \note           This function should be called once during application initialization
 *                 It's automatically called by check_console_window_resize_event()
 */
static bool
initialize_resize_detection(void) {
    struct sigaction sa;
    sa.sa_handler = sigwinch_handler;
    // Init a empty sa_mask, https://stackoverflow.com/questions/20684290/signal-handling-and-sigemptyset
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Restart interrupted system calls

    // sigaction for detecting signals, https://stackoverflow.com/questions/231912/what-is-the-difference-between-sigaction-and-signal/
    return (sigaction(SIGWINCH, &sa, NULL) == 0);
}

/**
 * \brief          Check if the console window has been resized
 *                 On Linux, this function uses SIGWINCH signal detection
 *                 combined with ioctl() to get the current terminal size.
 * 
 * \param[out]     info COORD structure to return new window size
 * \return         true if the console window has changed size, false if not
 * 
 * \note           On first call, this function initializes the SIGWINCH signal handler
 * \note           The function maintains static state to track size changes
 */
bool
check_console_window_resize_event(COORD* info) {
    // We hold the old screen size and initialization state between calls
    static short old_screen_w = 0, old_screen_h = 0;
    static bool initialized = false;
    short current_screen_w, current_screen_h;

    // Initialize signal handler on first call
    if (!initialized) {
        if (!initialize_resize_detection()) {
            return false;
        }
        initialized = true;

        // Get initial terminal size
        if (!get_terminal_size(&old_screen_w, &old_screen_h)) {
            return false;
        }
        return false; // Don't report resize on first call
    }

    // Always check current size, not just when signal is received
    // This handles cases where signals might be missed or coalesced
    if (!get_terminal_size(&current_screen_w, &current_screen_h)) {
        return false;
    }

    // Check if size actually changed
    if (current_screen_w != old_screen_w || current_screen_h != old_screen_h) {
        old_screen_w = current_screen_w;
        old_screen_h = current_screen_h;
        info->X = current_screen_w;
        info->Y = current_screen_h;

        // Reset the resize flag after processing
        s_terminal_resized = 0;
        return true;
    }

    // If we received a signal but size didn't change, reset the flag
    if (s_terminal_resized) {
        s_terminal_resized = 0;
    }

    return false;
}

#endif