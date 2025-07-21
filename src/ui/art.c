#include "art.h"

/**
 * @brief ASCII art text for the birthday simulation.
 *
 * "BIRTHDAY SIMULATION" in a stylized format.
 */
const char* const BIRTHDAY_SIMULATION_ART_TEXT[] = {
    "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄",
    "██ ▄▄▀██▄██ ▄▄▀█▄ ▄█ ████ ▄▀█ ▄▄▀█ ██ ████ ▄▄▄ ██▄██ ▄▀▄ █ ██ █ ██ ▄▄▀█▄ ▄██▄██▀▄▄▀█ ▄▄▀█",
    "██ ▄▄▀██ ▄█ ▀▀▄██ ██ ▄▄ █ █ █ ▀▀ █ ▀▀ ████▄▄▄▀▀██ ▄█ █▄█ █ ██ █ ██ ▀▀ ██ ███ ▄█ ██ █ ██ █",
    "██ ▀▀ █▄▄▄█▄█▄▄██▄██▄██▄█▄▄██▄██▄█▀▀▀▄████ ▀▀▀ █▄▄▄█▄███▄██▄▄▄█▄▄█▄██▄██▄██▄▄▄██▄▄██▄██▄█",
    "▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"};

/**
 * @brief The number of lines in the birthday simulation art text.
 *
 */
const unsigned short BIRTHDAY_SIMULATION_ART_TEXT_LINE = ARRAY_SIZE(BIRTHDAY_SIMULATION_ART_TEXT);

void
art_text_center_render(WINDOW* win, int max_y, int max_x) {
    int start_y, start_x, max_text_width = 0;

    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to art_text_center_render is null");
    }

    if (max_y == 0) {
        max_y = getmaxy(win);
    }

    if (max_x == 0) {
        max_x = getmaxx(win);
    }

    // Calculate the maximum width of the art text
    for (int i = 0; i < BIRTHDAY_SIMULATION_ART_TEXT_LINE; i++) {
        int current_width = strlen(BIRTHDAY_SIMULATION_ART_TEXT[i]) / 3; // Divide by 3 for UTF-8
        if (current_width > max_text_width) {
            max_text_width = current_width;
        }
    }

    // Calculate the starting position to center the text
    start_y = (max_y - BIRTHDAY_SIMULATION_ART_TEXT_LINE) / 2 - 10;
    start_x = (max_x - max_text_width) / 2;

    // Ensure the starting positions are not negative
    if (start_y < 0) {
        start_y = 0;
    }
    if (start_x < 0) {
        start_x = 0;
    }

    for (int i = 0; i < BIRTHDAY_SIMULATION_ART_TEXT_LINE; i++) {
        mvwprintw(win, start_y + i, start_x, "%s", BIRTHDAY_SIMULATION_ART_TEXT[i]);
    }

    // Refresh the window to show the changes
    wrefresh(win);
}