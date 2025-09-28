/**
 * \file            paradox_form.c
 * \brief           The main birthday paradox function to create, render, resize
 *                  handle input, execute logic and display results.
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "paradox_form.h"

#define ACTION_SUBMIT 1

static const struct FormButton const paradox_form_buttons_metadata[] = {
    {"[ Run Simulation ]", "[ Running... ]", ACTION_SUBMIT},
};
static const unsigned short paradox_form_buttons_metadata_len =
    ARRAY_SIZE(paradox_form_buttons_metadata);

static const struct FormInputField const paradox_form_fields_metadata[] = {
    {"Domain Size (days)", 365, 5}, {"Sample Count (people)", 23, 9}, {"Simulation Runs", 1000, 5}};
static const unsigned short paradox_form_fields_metadata_len =
    ARRAY_SIZE(paradox_form_fields_metadata);

static form_manager_t* manager = NULL;

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/

static FIELD*
paradox_form_field_get(int index) {
    return manager->fields[index];
}

/**
 * \brief          Create a sub window from the parent window for the form
 *
 * \param[in]      win The window that will contain the created subwin for the form. This
 *                 should ideally be the content win
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 */
static void
paradox_form_create_sub_win(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to paradox_form_create_sub_win is null");
    }

    if (manager->sub_win) {
        delwin(manager->sub_win);
        manager->sub_win = NULL;
    }

    const int sub_win_rows_count = BH_FORM_Y_PADDING + // One border row, one spacing row
                                   paradox_form_fields_metadata_len + // Number of input
                                   1 +                                // One spacing row
                                   1 +                                // Button Row
                                   BH_FORM_Y_PADDING; // One border row, one spacing row

    // Create a sub-window for the form with extra space for the button
    manager->sub_win = derwin(win, sub_win_rows_count, max_x - 4, 1, 1);
    keypad(manager->sub_win, TRUE);

    set_form_win(manager->form, win);
    set_form_sub(manager->form, manager->sub_win);

    unpost_form(manager->form); // Safeguard against state issues
    post_form(manager->form);
}

/**
 * \brief          Take the value from the form field as arguments for calculating the
 *                 collision and simulation. The result will be stored back to the arguments
 *                 provided to the function.
 *
 * \param[in]      collision_probability A reference of the value of the probability of
 *                 hash collision of this result
 * \param[in]      simulated_runs_results A reference of the value of the actual collision
 *                 occur in simulations
 */
static void
run_simulation_from_input(double* collision_probability, double* simulated_runs_results) {
    int domain_size = atoi(field_buffer(paradox_form_field_get(0), 0));
    int sample_count = atoi(field_buffer(paradox_form_field_get(1), 0));
    int simulation_runs = atoi(field_buffer(paradox_form_field_get(2), 0));

    *collision_probability = calculate_birthday_collision_probability(domain_size, sample_count);
    *simulated_runs_results =
        simulate_birthday_collision(domain_size, sample_count, simulation_runs);

    if (*simulated_runs_results == -1.0) {
        render_full_page_error_exit(
            stdscr, 0, 0,
            "Memory allocation for simulate_birthday_collision fails at run_simulation_from_input");
    }
}

/**
 * \brief          Render the simulation result given the value in the arguments
 *
 * \param[in]      win The window that it will render the content on, which also contain
 *                 the sub win that holds the form. It will assume that the subwin is at the top
 *                 of the window and will render the content {BH_FORM_Y_PADDING} under it
 * \param[in]      collision_probability The result of the collision probability to render
 * \param[in]      simulated_runs_results The result of the simulations to render
 */
static void
render_simulation_result(WINDOW* win, double collision_probability, double simulated_runs_results) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_simulation_result is null");
    }

    int form_win_x, form_win_y;
    getmaxyx(manager->sub_win, form_win_y, form_win_x);

    uint8_t starting_rows = form_win_y + BH_FORM_Y_PADDING;
    uint8_t starting_cols = BH_FORM_X_PADDING + 1;

    mvwprintw(win, starting_rows, starting_cols, "Estimated chance of a collision:         ");
    mvwprintw(win, starting_rows, starting_cols, "Estimated chance of a collision: %.2f%%  ",
              collision_probability * 100);

    mvwprintw(win, starting_rows + 1, starting_cols, "Simulated runs results:         ");
    mvwprintw(win, starting_rows + 1, starting_cols, "Simulated runs results: %.2f%%  ",
              simulated_runs_results);

    wrefresh(win);
}

/**
 * \brief          Loop over all field and validate the field. Error message will
 *                 be displayed at the side of the field if any
 *
 * \return         true No error found, all field is valid
 * \return         false One or more input is invalid
 */
static bool
paradox_form_validate_all_fields() {
    bool all_valid = true;
    for (int i = 0; i < paradox_form_fields_metadata_len; i++) {
        set_current_field(manager->form, paradox_form_field_get(i));
        int result = form_driver(manager->form, REQ_VALIDATION);
        unsigned short longest_max_length_pad = calculate_longest_max_length(
            paradox_form_fields_metadata, paradox_form_fields_metadata_len, true);

        if (result == E_INVALID_FIELD) {
            all_valid = false;
            display_field_error(manager, paradox_form_field_get(i),
                                paradox_form_fields_metadata[i].max_length, true);
        }
    }

    if (all_valid) {
        set_current_field(manager->form, paradox_form_field_get(paradox_form_fields_metadata_len));
    } else {
        set_current_field(manager->form, paradox_form_field_get(0));
    }

    return all_valid;
}

/****************************************************************
                       EXTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Initializes the paradox form with the given window.
 *
 * \param[in]      win The window to display the form in. This should ideally be
 *                 the content window.
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 */
void
paradox_form_init(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0, "The window passed to paradox_form_init is null");
    }

    if (manager && manager->form) {
        render_full_page_error_exit(win, 0, 0,
                                    "The paradox form has already been initialized and another "
                                    "attempt to initialize is not permitted");
    }

    manager = create_form_manager(paradox_form_fields_metadata, paradox_form_fields_metadata_len,
                                  paradox_form_buttons_metadata, paradox_form_buttons_metadata_len);
    if (!manager) {
        render_full_page_error_exit(win, 0, 0,
                                    "Unable to allocate memory for paradox form manager");
        return;
    }

    // Create form fields for each paradox field
    for (unsigned short i = 0; i < manager->input_count; ++i) {
        const struct FormInputField* metadata = &paradox_form_fields_metadata[i];

        manager->fields[i] =
            new_field(1,                             // Field height
                      manager->max_field_length + 1, // Field width
                      i + BH_FORM_Y_PADDING,         // Field y-position
                      BH_FORM_X_PADDING + BH_FORM_FIELD_BRACKET_PADDING + manager->max_label_length
                          + BH_FORM_FIELD_BRACKET_PADDING, // Field x-position
                      0,                                   // number of offscreen rows
                      0                                    // number of additional working buffers
            );

        if (!manager->fields[i]) {
            // Cleanup on failure
            for (int j = 0; j < i; j++) {
                free_field(manager->fields[j]);
            }
            free(manager->fields);
            free(manager->trackers);
            free(manager);

            render_full_page_error_exit(win, 0, 0,
                                        "Unable to allocate memory for paradox form field");
            return;
        }

        // Convert the default value to string
        char* string_buffer = (char*)malloc(sizeof(char) * metadata->max_length + 1);
        if (!string_buffer) {
            render_full_page_error_exit(
                stdscr, 0, 0, "Memory allocation failed for hash paradox form default value");
        }

        snprintf(string_buffer, metadata->max_length + 1, "%hu", metadata->default_value);

        // Make the field visible and editable
        field_opts_on(manager->fields[i], O_STATIC);    // Keep field static size
        field_opts_off(manager->fields[i], O_AUTOSKIP); // Don't auto skip to next field
        set_field_back(manager->fields[i],
                       A_NORMAL); // Set normal background for all fields initially
        set_field_buffer(manager->fields[i], 0,
                         string_buffer);                  // Set the default value for the field
        set_field_just(manager->fields[i], JUSTIFY_LEFT); // Left justify the content

        // Set maximum field length
        set_max_field(manager->fields[i], manager->max_field_length);

        // Set the field type to numeric
        int max_value = calculate_form_max_value(metadata->max_length);
        set_field_type(manager->fields[i], TYPE_INTEGER, 0, (long)1, (long)max_value);

        // Initialize tracker
        manager->trackers[i].field = manager->fields[i];
        manager->trackers[i].current_length = strlen(string_buffer);
        manager->trackers[i].max_length = metadata->max_length;
        manager->trackers[i].field_index = i;
        manager->trackers[i].cursor_position = manager->trackers[i].current_length;

        free(string_buffer);
    }

    // Create the submit button field
    manager->fields[manager->input_count] = create_button_field(
        paradox_form_buttons_metadata[0].label, // Button label
        paradox_form_fields_metadata_len + BH_FORM_Y_PADDING + 1, BH_FORM_X_PADDING);

    // Create the form
    manager->form = new_form(manager->fields);
    paradox_form_create_sub_win(win, max_y, max_x);
}

/**
 * \brief          Renders the paradox form in the given window.
 *
 * \param[in]      win The window to render the form in. This should ideally be
 * the content window.
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 */
FORM*
paradox_form_render(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to paradox_form_render is null");
    }

    if (!manager || (manager && manager->form == NULL)) {
        paradox_form_init(win, max_y, max_x); // Initialize the form if not already done
    }

    // Set the label for the field
    for (unsigned short i = 0; i < manager->input_count; ++i) {
        mvwprintw(manager->sub_win, i + BH_FORM_Y_PADDING, BH_FORM_X_PADDING,
                  paradox_form_fields_metadata[i].label);
        mvwprintw(manager->sub_win, i + BH_FORM_Y_PADDING,
                  BH_FORM_X_PADDING + manager->max_label_length, ": [");
        mvwprintw(manager->sub_win, i + BH_FORM_Y_PADDING,
                  BH_FORM_X_PADDING + manager->max_label_length + BH_FORM_FIELD_BRACKET_PADDING + 1
                      + (manager->max_field_length + 1) + BH_FORM_FIELD_BRACKET_PADDING,
                  "]");
    }

    set_current_field(manager->form, manager->fields[0]);
    update_field_highlighting(manager);
    form_driver(manager->form, REQ_END_LINE);

    wrefresh(win);

    return manager->form;
}

/**
 * \brief          Restore the form to the window, that has previously
 *                 been cleared
 *
 * \param[in]      win The window that should restore the form to.
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 * \param[in]      collision_probability The previous result of to render
 * \param[in]      simulated_runs_results The previous result of to render
 */
void
paradox_form_restore(WINDOW* win, int max_y, int max_x, double collision_probability,
                     double simulated_runs_results) {
    if (!manager->form) {
        return;
    }

    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to paradox_form_restore is null");
    }

    // recreate the sub win
    paradox_form_create_sub_win(win, max_y, max_x);

    // Force re-render of field labels
    paradox_form_render(win, max_y, max_x);

    // Manually restore field buffers
    for (int i = 0; manager->fields[i] != NULL; ++i) {
        const char* buf = field_buffer(manager->fields[i], 0);
        set_field_buffer(manager->fields[i], 0, buf); // Force internal repaint
    }

    // Force redraw current field again
    set_current_field(manager->form, manager->fields[0]);
    form_driver(manager->form, REQ_FIRST_FIELD);

    if (collision_probability != -1 && simulated_runs_results != -1) {
        render_simulation_result(win, collision_probability, simulated_runs_results);
    }

    wrefresh(manager->sub_win);
}

/**
 * \brief          Destroys the paradox form and frees the memory allocated for it.
 *
 */
void
paradox_form_destroy() {
    free_form_manager(manager);
    manager = NULL;
}

/**
 * \brief          Handles input for the paradox form.
 *
 * \param[in]      win The window that the form previously initialize and render in.
 * \param[in]      ch The current int character input from the key pressed
 * \param[out]      collision_probability The variable reference to store the results of
 *                 collision probability
 * \param[out]      simulated_runs_results The variable reference to store the results of
 *                 simulations
 */
void
paradox_form_handle_input(WINDOW* win, int ch, double* collision_probability,
                          double* simulated_runs_results) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to paradox_form_handle_input is null");
    }

    FIELD* active_field = current_field(manager->form);
    int current_index = field_index(active_field);
    bool is_button = is_field_button(manager, current_index);

    int field_max_length = is_button ? 0 : manager->trackers[current_index].max_length;
    int longest_max_length_pad = manager->max_field_length + 1;

    switch (ch) {
        case KEY_UP:
        case KEY_DOWN: {
            int result = form_driver(manager->form, REQ_VALIDATION);

            if (result == E_INVALID_FIELD) {
                display_field_error(manager, active_field, field_max_length, true);
            } else {
                clear_field_error(manager, active_field);

                FIELD* old_field = active_field;
                if (ch == KEY_DOWN) {
                    form_driver(manager->form, REQ_NEXT_FIELD);
                } else {
                    form_driver(manager->form, REQ_PREV_FIELD);
                }
                form_driver(manager->form, REQ_END_LINE);

                active_field = current_field(manager->form);
                current_index = field_index(active_field);

                update_field_highlighting(manager);
                
                if (!is_button) {
                    on_field_change(manager, old_field, active_field);
                    pos_form_cursor(manager->form);
                } else {
                    set_field_buffer(paradox_form_field_get(manager->input_count), 0,
                                     paradox_form_buttons_metadata[0].label);
                    pos_form_cursor(manager->form);
                }
            }

        } break;

        case KEY_LEFT:
            bool can_move_left = cursor_can_move_left(manager, active_field);
            if (!is_button && can_move_left) {
                int result = form_driver(manager->form, REQ_PREV_CHAR);
                if (result == E_OK) {
                    unsigned int cursor_pos = get_cursor_position(manager, active_field);
                    if (cursor_pos > 0) {
                        decrement_cursor_position(manager, active_field);
                    }
                }
            }
            break;
        case KEY_RIGHT:
            bool can_move_right = cursor_can_move_right(manager, active_field);
            if (!is_button && can_move_right) {
                int result = form_driver(manager->form, REQ_NEXT_CHAR);
                if (result == E_OK) {
                    unsigned int cursor_pos = get_cursor_position(manager, active_field);
                    if (cursor_pos < get_field_current_length(manager, active_field)) {
                        increment_cursor_position(manager, active_field);
                    }
                }
            }
            break;

        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (!is_button && get_field_current_length(manager, active_field) > 0) {
                unsigned short prev_length = get_field_length_on_screen(manager, active_field);
                int result = form_driver(manager->form, REQ_DEL_PREV);
                if (result == E_OK) {
                    unsigned short new_length = get_field_length_on_screen(manager, active_field);

                    if (new_length != prev_length) {
                        decrement_field_length(manager, active_field);
                        // Whenever a character is deleted, the cursor position is automatically
                        // move to the left by ncurses, so we need to update our tracker as well
                        decrement_cursor_position(manager, active_field);
                    }
                }
            }
            break;
        case KEY_DC:
            if (!is_button && get_field_current_length(manager, active_field) > 0) {
                unsigned short prev_length = get_field_length_on_screen(manager, active_field);
                int result = form_driver(manager->form, REQ_DEL_CHAR);
                if (result == E_OK) {
                    unsigned short new_length = get_field_length_on_screen(manager, active_field);

                    if (new_length != prev_length) {
                        decrement_field_length(manager, active_field);
                    }
                }
            }
            break;

        case '\n': {
            int result = form_driver(manager->form, REQ_VALIDATION);
            if (result == E_INVALID_FIELD) {
                display_field_error(manager, active_field, field_max_length, true);
            } else if (current_index == manager->input_count) {
                bool all_field_valid = paradox_form_validate_all_fields();
                if (all_field_valid) {
                    run_simulation_from_input(collision_probability, simulated_runs_results);
                    render_simulation_result(win, *collision_probability, *simulated_runs_results);
                }
            }
        } break;

        default: {
            if (!is_button && isdigit(ch)
                && field_has_space_for_char(manager, active_field)) {
                unsigned short prev_length = get_field_length_on_screen(manager, active_field);
                int result = form_driver(manager->form, ch);
                if (result == E_OK) {
                    unsigned short new_length = get_field_length_on_screen(manager, active_field);

                    if (new_length != prev_length) {
                        increment_field_length(manager, active_field);
                        // Whenever a character is added, the cursor position is automatically
                        // move to the right by ncurses, so we need to update our tracker as well
                        increment_cursor_position(manager, active_field);
                    }
                }
            }
        } break;
    }
}