#ifndef UTILS_H
#define UTILS_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#include <ncurses/ncurses.h>

/**
 * @brief Prints a string centered within a specified area of a window.
 *
 * This function calculates the horizontal center of the given width and prints
 * the provided string at that position within the specified window, row,
 * and with the given color attribute. If no window is provided, it defaults
 * to the standard screen.
 *
 * @param win The window to print in. If NULL, defaults to stdscr.
 * @param start_y The starting row (y-coordinate) to print on. If INT_MAX, uses the
 * current cursor y-position of the window.
 * @param start_x The starting column (x-coordinate) to begin centering from.
 * If INT_MAX, uses the current cursor x-position of the window.
 * @param width The width of the area to center the string within. If 0, defaults
 * to 80 columns.
 * @param string The null-terminated string to print.
 * @param color The color attribute to apply to the printed string (e.g., A_BOLD | COLOR_PAIR(1)).
 */
void print_in_middle(WINDOW *win, unsigned int start_y, unsigned int start_x, unsigned int width,
                     const char *string, chtype color);

/**
 * @brief Generate random input data for hash testing
 *
 * This function generates printable ASCII characters in the range of 32-126
 *
 * @param buffer Output buffer for random data
 * @param min_len Minimum length of random data
 * @param max_len Maximum length of random data
 * @return size_t Actual length of generated data
 */
size_t generate_random_input(uint8_t *buffer, size_t min_len, size_t max_len) ;
                     
/**
 * @brief Convert binary data to printable string representation
 *
 * @param data Binary data to convert
 * @param len Length of binary data
 * @param output Output buffer (should be at least len*4 + 1 bytes)
 */
void binary_to_string(const uint8_t *data, size_t len, char *output);

#define BH_MAIN_COLOR_PAIR 1
#define BH_ERROR_COLOR_PAIR 2
#define BH_SUCCESS_COLOR_PAIR 3
#define BH_WARNING_COLOR_PAIR 4
#define BH_INFO_COLOR_PAIR 5
#define BH_HIGHLIGHT_COLOR_PAIR 6

/**
 * @brief Initialize color pairs for ncurses
 *
 * This function initializes color pairs used in the application.
 * It should be called after initscr() and before any other ncurses functions.
 */
uint8_t init_color_pairs();

#endif