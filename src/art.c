#include <ncurses/ncurses.h>
#include <string.h>

#include "art.h"

const char *const BIRTHDAY_SIMULATION_ART_TEXT[] = {
    "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄",
    "██ ▄▄▀██▄██ ▄▄▀█▄ ▄█ ████ ▄▀█ ▄▄▀█ ██ ████ ▄▄▄ ██▄██ ▄▀▄ █ ██ █ ██ ▄▄▀█▄ ▄██▄██▀▄▄▀█ ▄▄▀",
    "██ ▄▄▀██ ▄█ ▀▀▄██ ██ ▄▄ █ █ █ ▀▀ █ ▀▀ ████▄▄▄▀▀██ ▄█ █▄█ █ ██ █ ██ ▀▀ ██ ███ ▄█ ██ █ ██",
    "██ ▀▀ █▄▄▄█▄█▄▄██▄██▄██▄█▄▄██▄██▄█▀▀▀▄████ ▀▀▀ █▄▄▄█▄███▄██▄▄▄█▄▄█▄██▄██▄██▄▄▄██▄▄██▄██▄",
    "▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"};
const unsigned short BIRTHDAY_SIMULATION_ART_TEXT_LEN =
    sizeof(BIRTHDAY_SIMULATION_ART_TEXT) / sizeof(BIRTHDAY_SIMULATION_ART_TEXT[0]);

/**
 * @brief Print the birthday simulation art text in the given window.
 *
 * @param win The ncurses window to print in. Default is stdscr if NULL.
 */
void ncurses_print_art_text(WINDOW *win) {
  int max_y, max_x;
  int start_y, start_x;
  int text_height = BIRTHDAY_SIMULATION_ART_TEXT_LEN;
  int max_text_width = 0;

  if (win == NULL)
    win = stdscr;

  // Get the size of the window
  getmaxyx(win, max_y, max_x);

  // Calculate the maximum width of the art text
  for (int i = 0; i < text_height; i++) {
    int current_width = strlen(BIRTHDAY_SIMULATION_ART_TEXT[i]);
    if (current_width > max_text_width) {
      max_text_width = current_width;
    }
  }

  // Calculate the starting position to center the text
  start_y = (max_y - text_height) / 2 - 10;
  start_x = (max_x - max_text_width) / 2;

  // Ensure the starting positions are not negative
  if (start_y < 0)
    start_y = 0;
  if (start_x < 0)
    start_x = 0;

  for (int i = 0; i < BIRTHDAY_SIMULATION_ART_TEXT_LEN; i++) {
    mvwprintw(win, start_y + i, start_x, "%s", BIRTHDAY_SIMULATION_ART_TEXT[i]);
  }
  wrefresh(win);
}