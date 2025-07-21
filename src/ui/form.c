#include "form.h"

/**
 * \brief          Calculates the longest max_length from the paradox_fields array.
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