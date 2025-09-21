/**
 * \file            form.c
 * \brief           A generic helper function for ncurses form
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "form.h"

/****************************************************************
                       UTILITY FUNCTIONS
****************************************************************/

/**
 * \brief          Calculates the longest max_length from the ncurses form fields array.
 * \param[in]      form_fields The array of form input fields.
 * \param[in]      form_fields_len The length of the form_fields array.
 * \param[in]      padding Whether to add a padding to the longest max_length. The padding
 *                 will return the longest max_length + the extra width of the field.
 *
 * \return         The longest max_length value.
 */
unsigned short
calculate_longest_max_length(const struct FormInputField const form_fields[],
                             uint8_t form_fields_len, bool padding) {
    unsigned short longest = 0;

    for (unsigned short i = 0; i < form_fields_len; ++i) {
        if (form_fields[i].max_length > longest) {
            longest = form_fields[i].max_length;
        }
    }

    if (padding) {
        longest++;
    }

    return longest;
}

/**
 * \brief          Calculates the maximum value based on the length of the field.
 * \example        If max_length is 3, max_value is 999
 *
 * \param[in]      length The length of the field.
 * \return         The maximum value based on the length of the field.
 */
int
calculate_form_max_value(int length) {
    if (length <= 0) {
        return 0; // Or handle as an error
    }

    // pow(10, length) - 1 gives the largest number with 'length' digits
    // Example: pow(10, 3) - 1 = 1000 - 1 = 999
    return (int)pow(10, length) - 1;
}

/****************************************************************
                        BUTTON FUNCTIONS
****************************************************************/

/**
 * \brief          Create a button field object
 *
 * \param[in]      label The string label for the button.
 * \param[in]      frow The row where the button will be placed.
 * \param[in]      fcol The column where the button will be placed.
 * \return         The created button field.
 */
FIELD*
create_button_field(const char* label, unsigned short frow, unsigned short fcol) {
    unsigned short button_width = strlen(label);

    FIELD* button_field = new_field(1,            // Field height
                                    button_width, // Field width
                                    frow,         // First row
                                    fcol,         // First column
                                    0,            // No offscreen rows
                                    0);           // No additional working buffers

    set_field_buffer(button_field, 0, label);
    field_opts_off(button_field, O_EDIT); // Make the button field non-editable
    set_field_back(button_field, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));

    return button_field;
}

/**
 * \brief          Update the button field to indicate whether it is in a running state or not.
 *
 * \param[in]      button_field The button field to update.
 * \param[in]      label The label to display when the button is not running.
 * \param[in]      running_label The label to display when the button is running.
 * \param[in]      is_running Whether the button is in a running state (true) or not (false).
 */
void
update_button_field_is_running(FIELD* button_field, const char* label, const char* running_label,
                               bool is_running) {
    if (button_field == NULL) {
        return;
    }

    if (is_running) {
        set_field_buffer(button_field, 0, running_label);
        set_field_back(button_field, A_BOLD | COLOR_PAIR(BH_WARNING_COLOR_PAIR));
    } else {
        set_field_buffer(button_field, 0, label);
        set_field_back(button_field, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
    }
}

/****************************************************************
                      FORM FIELD FUNCTIONS
****************************************************************/

/**
 * \brief          Updates the highlighting of the fields in the form based on the current field.
 *
 * \param[in]      current_form The current form to update.
 * \param[in]      form_field_count The number of fields in the form. This includes both input fields and
 *                 buttons.
 * \param[in]      form_button_indexes The indexes of the button fields in the form. The indexes
 *                 MUST be sorted in ascending order.
 * \param[in]      form_button_indexes_len The length of the button indexes array.
 */
void
update_field_highlighting(FORM* current_form, unsigned short form_field_count,
                          unsigned short form_button_indexes[],
                          unsigned short form_button_indexes_len) {
    if (current_form == NULL || form_field_count == 0 || form_button_indexes == NULL) {
        return;
    }

    FIELD** fields = form_fields(current_form);
    FIELD* current = current_field(current_form);
    int current_index = field_index(current);

    // Show or hide cursor based on whether we're on the button
    bool is_button = binary_search(form_button_indexes, form_button_indexes_len, current_index);
    if (is_button) {
        curs_set(0); // Hide cursor on button
    } else {
        curs_set(1); // Show cursor on input fields
    }

    for (unsigned short i = 0; i < form_field_count + 1; ++i) {
        // If the field is the current one, highlight it
        if (fields[i] == current) {
            bool is_button = binary_search(form_button_indexes, form_button_indexes_len, i);
            if (is_button) {
                // Button selected - invert colors
                set_field_back(fields[i], A_REVERSE | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
            } else {
                // Input field selected
                set_field_back(fields[i], A_REVERSE);
            }
        } else {
            // If the field is not the current one, set normal colors
            bool is_button = binary_search(form_button_indexes, form_button_indexes_len, i);
            if (is_button) {
                // Button not selected - normal button colors
                set_field_back(fields[i], A_NORMAL | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
            } else {
                // Input field not selected
                set_field_back(fields[i], A_NORMAL);
            }
        }
    }

    form_driver(current_form, REQ_VALIDATION); // Force form refresh
}

/**
 * \brief          Displays an error message for a specific field in the form on the right side
 *
 * \param[in]      sub_win The sub-window where the form is displayed.
 * \param[in]      field The field to display the error for.
 * \param[in]      field_index The index of the field in the form's field array.
 * \param[in]      max_label_length The maximum length of the field label of the form.
 * \param[in]      max_field_length The maximum length of the field value of the form.
 * \param[in]      max_field_value The maximum value allowed for the current field based on its max_length.
 * \param[in]      y_padding Whether to apply BH_FORM_Y_PADDING to the error message.
 */
void
display_field_error(WINDOW* sub_win, FIELD* field, int field_index, unsigned short max_label_length,
                    unsigned short max_field_length, int max_field_value, bool y_padding) {
    clear_field_error(sub_win, field_index, max_label_length, max_field_length);

    char* buffer = field_buffer(field, 0);
    int value;

    int y_pos = field_index;
    if (y_padding) {
        y_pos += BH_FORM_Y_PADDING; // Apply vertical padding if needed
    }

    int x_pos = BH_FORM_X_PADDING + max_label_length + BH_FORM_FIELD_BRACKET_PADDING + 1
                + max_field_length + BH_FORM_FIELD_BRACKET_PADDING + 2;

    // Clear any previous error message first
    mvwprintw(sub_win, y_pos, x_pos, "                    ");

    // Trim trailing spaces from buffer
    char* end = buffer + strlen(buffer) - 1;
    while (end > buffer && isspace(*end)) {
        *end = '\0';
        end--;
    }

    // If empty after trimming, don't show error
    if (strlen(buffer) == 0) {
        return;
    }

    // Try to convert the buffer to a number
    if (sscanf(buffer, "%d", &value) != 1) {
        wattron(sub_win, COLOR_PAIR(BH_ERROR_COLOR_PAIR));
        mvwprintw(sub_win, y_pos, x_pos, "Must be a number");
        wattroff(sub_win, COLOR_PAIR(BH_ERROR_COLOR_PAIR));
        return;
    }

    // Get the maximum allowed value based on the field's max length
    int min = 1;
    int max = max_field_value;

    if (value < min || value > max) {
        wattron(sub_win, COLOR_PAIR(BH_ERROR_COLOR_PAIR));
        mvwprintw(sub_win, y_pos, x_pos, "Range: %d-%d", min, max);
        wattroff(sub_win, COLOR_PAIR(BH_ERROR_COLOR_PAIR));
    }
}

/**
 * \brief          Clears the error message for a specific field in the form created by
 *                 display_field_error.
 *
 * \param[in]      sub_win The sub-window where the form is displayed.
 * \param[in]      field_index The index of the field in the form's field array.
 * \param[in]      max_label_length The maximum length of the field label of the form.
 * \param[in]      max_field_length The maximum length of the field value of the form.
 *
 */
void
clear_field_error(WINDOW* sub_win, int field_index, unsigned short max_label_length,
                  unsigned short max_field_length) {
    mvwprintw(sub_win, field_index + BH_FORM_Y_PADDING,
              BH_FORM_X_PADDING + max_label_length + +BH_FORM_FIELD_BRACKET_PADDING + 1
                  + max_field_length + BH_FORM_FIELD_BRACKET_PADDING + 2,
              "                    ");
}

/****************************************************************
                         FORM MANAGERS
****************************************************************/

/**
 * \brief          Create form manager
 */
form_manager_t*
create_paradox_form_manager(const struct FormInputField const input_metadata[],
                            unsigned short input_metadata_len,
                            const struct FormButton const button_metadata[],
                            unsigned short button_metadata_len) {
    form_manager_t* manager = malloc(sizeof(form_manager_t));
    if (!manager) {
        return NULL;
    }

    unsigned short total_field_count = input_metadata_len + button_metadata_len;

    // Allocate arrays
    manager->fields = malloc(sizeof(FIELD*) * (total_field_count + 1)); // +1 for NULL terminator
    manager->trackers = malloc(sizeof(field_tracker_t) * input_metadata_len);

    manager->input_count = input_metadata_len;
    manager->button_count = button_metadata_len;
    manager->total_field_count = total_field_count;

    manager->max_label_length = 0;
    manager->max_field_length =
        calculate_longest_max_length(input_metadata, input_metadata_len, false);

    manager->form = malloc(sizeof(FORM*));
    manager->sub_win = malloc(sizeof(WINDOW*));

    // Find the longest label length for the fields
    for (unsigned short i = 0; i < input_metadata_len; ++i) {
        unsigned short label_length = strlen(input_metadata[i].label);
        if (label_length > manager->max_label_length) {
            manager->max_label_length = label_length;
        }
    }

    if (!manager->fields || !manager->trackers) {
        free(manager->fields);
        free(manager->trackers);
        free(manager);
        return NULL;
    }

    // Add NULL terminator at the end
    manager->fields[total_field_count + 1] = NULL;

    return manager;
}

/**
 * \brief          Find tracker for a given field
 */
field_tracker_t*
find_field_tracker(form_manager_t* manager, FIELD* field) {
    if (!manager || !field) {
        return NULL;
    }

    for (int i = 0; i < manager->total_field_count; i++) {
        if (manager->trackers[i].field == field) {
            return &manager->trackers[i];
        }
    }
    return NULL;
}

/**
 * \brief          Check if field has space for another character
 */
bool
field_has_space_for_char(form_manager_t* manager, FIELD* field) {
    if (!manager || !field) {
        return false;
    }

    field_tracker_t* tracker = find_field_tracker(manager, field);
    if (!tracker) {
        return false;
    }

    return tracker->current_length < tracker->max_length;
}

/**
 * \brief          Increment field character count
 */
void
increment_field_length(form_manager_t* manager, FIELD* field) {
    if (!manager || !field) {
        return;
    }

    field_tracker_t* tracker = find_field_tracker(manager, field);
    if (tracker && tracker->current_length < tracker->max_length) {

        tracker->current_length++;
    }
}

/**
 * \brief          Decrement field character count
 */
void
decrement_field_length(form_manager_t* manager, FIELD* field) {
    if (!manager || !field) {
        return;
    }

    field_tracker_t* tracker = find_field_tracker(manager, field);
    if (tracker && tracker->current_length > 0) {
        tracker->current_length--;
    }
}

/**
 * \brief          Reset field character count (useful when setting field buffer directly)
 */
void
reset_field_length(form_manager_t* manager, FIELD* field) {
    if (!manager || !field) {
        return;
    }

    field_tracker_t* tracker = find_field_tracker(manager, field);
    if (!tracker) {
        return;
    }

    // Count actual characters in the field buffer
    char* buffer = field_buffer(field, 0);
    if (buffer) {
        int len = strlen(buffer);
        // Remove trailing spaces
        while (len > 0 && isspace((unsigned char)buffer[len - 1])) {
            len--;
        }
        tracker->current_length = len;
    } else {
        tracker->current_length = 0;
    }
}

/**
 * \brief          Get current length of field by checking what is visible on screen
 */
unsigned short
get_field_length_on_screen(form_manager_t* manager, FIELD* field) {

    WINDOW* sub = form_sub(manager->form);
    int fy, fx, h, w, off, nb;
    field_info(field, &h, &w, &fy, &fx, &off, &nb);

    // We add one extra character because all input fields have an extra
    // character for the cursor position.
    // Then for the another additional character for the null terminator
    // added automatically by ncurses.
    char* visible = calloc(manager->max_field_length + 1 + 1, sizeof(char));
    if (!visible) {
        return 0;
    }

    int copied = mvwinnstr(sub, fy, fx, visible, w);
    if (copied < 0) {
        copied = 0;
    }
    if (copied > manager->max_field_length) {
        copied = manager->max_field_length;
    }
    visible[copied] = '\0';

    // Get actual length without trailing spaces
    char* end = visible + strlen(visible);
    while (end > visible && isspace((unsigned char)*(end - 1))) {
        *end = '\0';
        end--;
    }
    *end = '\0';

    unsigned short length = (unsigned short)(end - visible);
    free(visible);
    return length;
}

/**
 * \brief          Get current length of field
 */
int
get_field_current_length(form_manager_t* manager, FIELD* field) {
    if (!manager || !field) {
        return -1;
    }

    field_tracker_t* tracker = find_field_tracker(manager, field);
    return tracker ? tracker->current_length : -1;
}

/**
 * \brief          Handle field switching (call this when changing active field)
 */
void
on_field_change(form_manager_t* manager, FIELD* old_field, FIELD* new_field) {
    if (!manager || !new_field) {
        return;
    }

    // Sync the old field's length in case it was modified externally
    if (old_field) {
        reset_field_length(manager, old_field);
    }
}

/**
 * \brief          Cleanup form manager
 */
void
free_paradox_form_manager(form_manager_t* manager) {
    if (!manager) {
        return;
    }

    // Free form first
    if (manager->form) {
        unpost_form(manager->form);
        free_form(manager->form);
    }

    // Free fields
    if (manager->fields) {
        for (int i = 0; i < manager->total_field_count; i++) {
            if (manager->fields[i]) {
                free_field(manager->fields[i]);
            }
        }
        free(manager->fields);
    }

    // Free trackers
    free(manager->trackers);

    // Free sub window and form pointers
    free(manager->sub_win);
    free(manager->form);

    free(manager);
}
