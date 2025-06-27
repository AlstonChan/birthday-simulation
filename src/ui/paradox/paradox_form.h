#ifndef PARADOX_FORM_H
#define PARADOX_FORM_H

#include <ncurses/ncurses.h>
#include <ncurses/form.h>

#include "../form.h"

/**
 * @brief The structure for the input fields in the paradox form.
 * 
 */

extern const char const *paradox_form_button_text;
extern const struct FormInputField paradox_fields[];
extern const unsigned short paradox_fields_len;

FIELD *paradox_field_get(int index);
FIELD **paradox_field_get_all();
FORM *paradox_form_get();
WINDOW *paradox_form_sub_win_get();

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