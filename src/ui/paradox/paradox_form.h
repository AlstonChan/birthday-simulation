#ifndef PARADOX_FORM_H
#define PARADOX_FORM_H

#include <ncurses/ncurses.h>
#include <ncurses/form.h>

#include "../form.h"

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
 * @brief The padding between the field and the bracket.
 */
#define FORM_FIELD_BRACKET_PADDING 2

/**
 * @brief The structure for the input fields in the paradox form.
 * 
 */

extern const char const *paradox_form_button_text;
extern const struct FormInputField paradox_fields[];
extern const unsigned short paradox_fields_len;
extern unsigned short max_label_length;

FIELD *paradox_field_get(int index);
FIELD **paradox_field_get_all();
FORM *paradox_form_get();
WINDOW *paradox_form_sub_win_get();

/**
 * @brief Updates the highlighting of the paradox form fields based on the current field.
 *
 */
void update_field_highlighting();


/**
 * @brief Displays an error message for a field in the paradox form.
 *
 * @param field The field to display the error message for.
 * @param field_index The index of the field.
 */
void display_field_error(FIELD *field, int field_index);

/**
 * @brief Initializes the paradox form with the given window.
 * If no window is provided, it will return early without doing anything.
 *
 * @param win The window to display the form in. This should ideally be
 * the content window.
 */
void paradox_form_init(WINDOW *win);

/**
 * @brief Destroys the paradox form and frees the memory allocated for it.
 *
 */
void paradox_form_destroy();

/**
 * @brief Renders the paradox form in the given window.
 * If no window is provided, it will return early without doing anything.
 *
 * @param win The window to render the form in. This should ideally be
 * the content window.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 */
FORM *paradox_form_render(WINDOW *win, int max_y, int max_x);

/**
 * @brief Handles input for the paradox form.
 *
 */
void paradox_form_handle_input(WINDOW *win, int ch);

/**
 * @brief Clears the error message for a given field.
 *
 */
void paradox_form_clear_error_message(int field_index);

/**
 * @brief Validates all fields in the paradox form.
 *
 */
bool paradox_form_validate_all_fields(WINDOW *win);

#endif