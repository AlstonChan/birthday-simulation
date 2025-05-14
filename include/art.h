#ifndef ART_H
#define ART_H

extern const char *const BIRTHDAY_SIMULATION_ART_TEXT[];
extern const unsigned short BIRTHDAY_SIMULATION_ART_TEXT_LEN;

void ncurses_print_art_text(WINDOW *win);

#endif