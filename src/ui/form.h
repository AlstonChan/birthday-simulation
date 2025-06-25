#ifndef COMMON_FORM_H
#define COMMON_FORM_H

#include <ncurses/ncurses.h>
#include <ncurses/form.h>

/**
 * @brief The structure for the input fields in the paradox form.
 *
 */
struct FormInputField
{
    char *label;
    unsigned short default_value;
    int max_length;
};

/**
 * @brief The y-padding of the form relative to the parent window.
 *
 */
#define BH_FORM_Y_PADDING 2
/**
 * @brief The x-padding of the form relative to the parent window.
 *
 */
#define BH_FORM_X_PADDING 2

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
FIELD *create_button_field(const char *label, unsigned short frow, unsigned short fcol);

/**
 * @brief Calculates the maximum value based on the length of the field.
 * @example If max_length is 3, max_value is 999
 *
 * @param length The length of the field.
 * @return int The maximum value based on the length of the field.
 */
int calculate_form_max_value(int length);

#endif