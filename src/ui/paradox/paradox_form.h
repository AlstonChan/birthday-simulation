#ifndef PARADOX_FORM_H
#define PARADOX_FORM_H

#include <ncurses/ncurses.h>
#include <ncurses/form.h>

/**
 * @brief The y-padding of the form relative to the parent window.
 *
 */
#define FORM_Y_PADDING 2
/**
 * @brief The x-padding of the form relative to the parent window.
 *
 */
#define FORM_X_PADDING 2

/**
 * @brief The maximum length of a field in the paradox form.
 */
#define MAX_FIELD_LENGTH 8

/**
 * @brief The color pair for error messages.
 *
 */
#define ERROR_COLOR_PAIR 2

/**
 * @brief The color pair for the button.
 *
 */
#define BUTTON_COLOR_PAIR 3

/**
 * @brief The structure for the input fields in the paradox form.
 * 
 */
struct ParadoxInputField {
    char *label;  
    unsigned short default_value; 
    int max_length;  
};

  extern const char const *paradox_form_button_text;
  extern const struct ParadoxInputField paradox_fields[];
  extern const unsigned short paradox_fields_len;
  extern unsigned short max_label_length;

FIELD *paradox_field_get(int index);
FIELD **paradox_field_get_all();
FORM *paradox_form_get();
WINDOW *paradox_form_sub_win_get();

void update_field_highlighting();
void display_field_error(FIELD *field, int field_index);
void paradox_form_init(WINDOW *win);
void paradox_form_destroy();
FORM *paradox_form_render(WINDOW *win, int max_y, int max_x);

void paradox_form_handle_input(WINDOW *win, int ch);
void paradox_form_clear_error_message(int field_index);
bool paradox_form_validate_all_fields(WINDOW *win);

#endif