#ifndef COMMON_FORM_H
#define COMMON_FORM_H

#include <ctype.h>
#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <string.h>

#include "../utils/utils.h"

/**
 * @brief The structure for the input fields in the paradox form.
 *
 */
struct FormInputField {
    char* label;
    unsigned short default_value;
    int max_length;
};

/**
 * @brief The y-padding of the form relative to the parent window.
 *
 */
#define BH_FORM_Y_PADDING             2
/**
 * @brief The x-padding of the form relative to the parent window.
 *
 */
#define BH_FORM_X_PADDING             2

/**
 * @brief The padding between the field and the bracket.
 */
#define BH_FORM_FIELD_BRACKET_PADDING 2

/**
 * @brief Calculates the longest max_length from the paradox_fields array.
 * @param form_fields The array of form input fields.
 * @param form_fields_len The length of the form_fields array.
 * @param padding Whether to add a padding to the longest max_length. The padding
 * will return the longest max_length + the extra width of the field.
 *
 * @return unsigned short The longest max_length value.
 */
unsigned short calculate_longest_max_length(const struct FormInputField const form_fields[],
                                            uint8_t form_fields_len, bool padding);

/**
 * @brief Create a button field object
 *
 * @param label The string label for the button.
 * @param frow The row where the button will be placed.
 * @param fcol The column where the button will be placed.
 * @return FIELD* The created button field.
 */
FIELD* create_button_field(const char* label, unsigned short frow, unsigned short fcol);

/**
 * @brief Calculates the maximum value based on the length of the field.
 * @example If max_length is 3, max_value is 999
 *
 * @param length The length of the field.
 * @return int The maximum value based on the length of the field.
 */
int calculate_form_max_value(int length);

/**
 * @brief Updates the highlighting of the fields in the form based on the current field.
 *
 * @param current_form The current form to update.
 * @param form_field_count The number of fields in the form. This includes both input fields and
 * buttons.
 * @param form_button_indexes The indexes of the button fields in the form. The indexes
 *                            MUST be sorted in ascending order.
 * @param form_button_indexes_len The length of the button indexes array.
 */
void update_field_highlighting(FORM* current_form, unsigned short form_field_count,
                               unsigned short form_button_indexes[],
                               unsigned short form_button_indexes_len);

/**
 * @brief Displays an error message for a specific field in the form on the right side
 *
 * @param sub_win The sub-window where the form is displayed.
 * @param field The field to display the error for.
 * @param field_index The index of the field in the form's field array.
 * @param max_label_length The maximum length of the field label of the form.
 * @param max_field_length The maximum length of the field value of the form.
 * @param max_field_value The maximum value allowed for the current field based on its max_length.
 * @param y_padding Whether to apply BH_FORM_Y_PADDING to the error message.
 */
void display_field_error(WINDOW* sub_win, FIELD* field, int field_index,
                         unsigned short max_label_length, unsigned short max_field_length,
                         int max_field_value, bool y_padding);

/**
 * @brief Clears the error message for a specific field in the form created by
 * display_field_error.
 *
 * @param sub_win The sub-window where the form is displayed.
 * @param field_index The index of the field in the form's field array.
 * @param max_label_length The maximum length of the field label of the form.
 * @param max_field_length The maximum length of the field value of the form.
 *
 */
void clear_field_error(WINDOW* sub_win, int field_index, unsigned short max_label_length,
                       unsigned short max_field_length);

#endif