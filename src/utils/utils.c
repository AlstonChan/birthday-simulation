/**
 * \file            utils.c
 * \brief           Utility function that the application depends on
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "utils.h"

/**
 * \brief          Prints a string centered within a specified area of a window.
 *                 This function calculates the horizontal center of the given
 *                 width and prints the provided string at that position within
 *                 the specified window, row, and with the given color attribute.
 *                 If no window is provided, it defaults to the standard screen.
 *
 * \param[in]      win The window to print in. If NULL, defaults to stdscr.
 * \param[in]      start_y The starting row (y-coordinate) to print on. If INT_MAX, uses
 *                 the current cursor y-position of the window.
 * \param[in]      start_x The starting column (x-coordinate) to begin centering from.
 *                 If INT_MAX, uses the current cursor x-position of the window.
 * \param[in]      width The width of the area to center the string within. If 0, defaults
 *                 to 80 columns.
 * \param[in]      string The null-terminated string to print.
 * \param[in]      color The color attribute to apply to the printed string 
 *                 (e.g., A_BOLD | COLOR_PAIR(1)).
 */
void
print_in_middle(WINDOW* win, unsigned int start_y, unsigned int start_x, unsigned int width,
                const char* string, chtype color) {
    int length, x, y;
    float temp;

    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0, "The window passed to print_in_middle is null");
    }

    // Get the current cursor position of the window.
    getyx(win, y, x);

    // If a starting x-coordinate is provided, use it.
    if (start_x != INT_MAX) {
        x = start_x;
    }

    // If a starting y-coordinate is provided, use it.
    if (start_y != INT_MAX) {
        y = start_y;
    }

    // If no width is provided, default to 40 columns.
    if (width == 0) {
        width = 40;
    }

    // Calculate the length of the string.
    length = (int)strlen(string);

    // Calculate the horizontal offset to center the string.
    temp = (float)(width - length) / 2;
    x = start_x + (int)temp;

    // Apply the color attribute.
    wattron(win, color);

    // Print the string at the calculated position.
    mvwprintw(win, y, x, "%s", string);

    // Turn off the color attribute.
    wattroff(win, color);

    // Refresh the window to display the changes.
    refresh();
}

/**
 * \brief          Generate random input data for hash testing
 *                 This function generates printable ASCII characters in the
 *                 range of 32-126
 *
 * \param[in]      buffer Output buffer for random data
 * \param[in]      min_len Minimum length of random data
 * \param[in]      max_len Maximum length of random data
 * \return         size_t Actual length of generated data
 */
size_t
generate_random_input(uint8_t* buffer, size_t min_len, size_t max_len) {
    size_t len = min_len + (rand() % (max_len - min_len + 1));
    RAND_bytes(buffer, len); // fills with secure random bytes
    return len;
}

/**
 * \brief          Convert a byte array to a hexadecimal string
 *                 This function converts a byte array to a hexadecimal 
 *                 string representation. The output is null-terminated
 *                 and can be used for display or logging.
 *
 * \param[in]      data Pointer to the byte array
 * \param[in]      len Length of the byte array
 * \param[in]      uppercase If true, uses uppercase letters (A-F), otherwise
 *                 lowercase (a-f)
 * \return         Pointer to the hexadecimal string, caller must free it
 */
char*
bytes_to_hex(const uint8_t* data, size_t len, bool uppercase) {
    if (!data || len == 0) {
        return NULL;
    }

    // Allocate buffer: 2 characters per byte + null terminator
    char* hex = malloc((len * 2) + 1);
    if (!hex) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        sprintf(hex + (i * 2), uppercase ? "%02X" : "%02x", data[i]);
    }

    hex[len * 2] = '\0'; // Null-terminate the string
    return hex;
}

/**
 * \brief          Initialize color pairs for ncurses
 *                 This function initializes color pairs used in the application.
 *                 It should be called after initscr() and before any other ncurses
 *                 functions.
 * \return         1 if the terminal does not support colours, 0 if the colors pair
 *                 has successfully initialize
 */
uint8_t
init_color_pairs() {
    if (has_colors() == FALSE) { // Check if the terminal supports colors
        endwin();
        return 1;
    }
    start_color(); // Start color functionality

    // Initialize color pairs for ncurses
    init_pair(BH_MAIN_COLOR_PAIR, COLOR_CYAN, COLOR_BLACK);         // Main color
    init_pair(BH_ERROR_COLOR_PAIR, COLOR_RED, COLOR_BLACK);         // Error color
    init_pair(BH_SUCCESS_COLOR_PAIR, COLOR_GREEN, COLOR_BLACK);     // Success color
    init_pair(BH_WARNING_COLOR_PAIR, COLOR_YELLOW, COLOR_BLACK);    // Warning color
    init_pair(BH_INFO_COLOR_PAIR, COLOR_CYAN, COLOR_BLACK);         // Info color
    init_pair(BH_HIGHLIGHT_COLOR_PAIR, COLOR_MAGENTA, COLOR_BLACK); // Highlight color
    return 0;
}

/**
 * \brief          Perform a binary search on a sorted array
 *                 This function performs a binary search on a sorted array
 *                 to find the target value.
 *
 * \param[in]      arr The sorted array to search in
 * \param[in]      size The size of the array
 * \param[in]      target The value to search for
 * \return         true if the target is found, false otherwise
 */
bool
binary_search(unsigned short arr[], unsigned short size, unsigned short target) {
    unsigned short left = 0, right = size - 1;

    if (arr == NULL || size == 0) {
        return false; // Handle empty or null array
    }
    if (target < arr[left] || target > arr[right]) {
        return false; // Target is out of bounds
    }

    while (left <= right) {
        unsigned short mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            return true;
        }
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return false;
}

/**
 * \brief          Check if a number is prime
 *                 This function checks if a given number is prime.
 *
 * \param[in]      n The number to check
 * \return         true if the number is prime, false otherwise
 */
bool
is_prime(unsigned int n) {
    if (n < 2) {
        return false;
    }
    if (n == 2) {
        return true;
    }
    if (n % 2 == 0) {
        return false;
    }

    for (size_t i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

/**
 * \brief          Find the next prime number greater than or equal to n
 *                 This function finds the next prime number that is greater than or equal to n.
 *
 * \param[in]      n The starting number
 * \return         The next prime number greater than or equal to n
 */
unsigned int
next_prime(unsigned int n) {
    if (n < 2) {
        return 2;
    }

    // Make sure n is odd (except for 2)
    if (n > 2 && n % 2 == 0) {
        n++;
    }

    while (!is_prime(n)) {
        n += 2; // Only check odd numbers
    }
    return n;
}
