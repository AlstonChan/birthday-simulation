#ifndef PARADOX_FORM_H
#define PARADOX_FORM_H

#include <ncurses/ncurses.h>
#include <ncurses/form.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/paradox_math.h"
#include "../../utils/utils.h"
#include "../../ui/error.h"
#include "../form.h"

extern const char const *paradox_form_button_text;
extern const struct FormInputField paradox_fields[];
extern const unsigned short paradox_fields_len;

FIELD *paradox_form_field_get(int index);
FIELD **paradox_form_field_get_all();
FORM *paradox_form_get();
WINDOW *paradox_form_sub_win_get();

/**
 * @brief Initializes the paradox form with the given window.
 *
 * @param win The window to display the form in. This should ideally be
 * the content window.
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 */
void paradox_form_init(WINDOW *win, int max_y, int max_x);

/**
 * @brief Renders the paradox form in the given window.
 *
 * @param win The window to render the form in. This should ideally be
 * the content window.
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 */
FORM *paradox_form_render(WINDOW *win, int max_y, int max_x);

/**
 * @brief Destroys the paradox form and frees the memory allocated for it.
 *
 */
void paradox_form_destroy();

/**
 * @brief Handles input for the paradox form.
 *
 * @param win The window that the form previously initialize and render in.
 * @param ch The current int character input from the key pressed
 * @param collision_probability The variable reference to store the results of 
 * collision probability
 * @param simulated_runs_results The variable reference to store the results of 
 * simulations
 */
void paradox_form_handle_input(WINDOW *win, int ch, double *collision_probability,
                                      double *simulated_runs_results);

/**
 * @brief Restore the form to the window, that has previously
 * been cleared
 *
 * @param win The window that should restore the form to.
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 * @param collision_probability The previous result of to render
 * @param simulated_runs_results The previous result of to render
 */
void paradox_form_restore(WINDOW *win, int max_y, int max_x, double collision_probability,
                                      double simulated_runs_results);

#endif