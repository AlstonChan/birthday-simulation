#ifndef PARADOX_H
#define PARADOX_H

#include <ncurses/ncurses.h>

/**
 * @brief The structure for the input fields in the paradox form.
 * 
 */
struct ParadoxInputField {
    char *label;  
    unsigned short default_value; 
    int max_length;  
};

void render_paradox_page(WINDOW *win, int max_y, int max_x);
void paradox_form_init(WINDOW *win);
void paradox_form_render(WINDOW *win, int max_y, int max_x);

#endif 