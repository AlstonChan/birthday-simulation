/**
 * \file            form.h
 * \brief           Header file for form.c
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef COMMON_FORM_H
#define COMMON_FORM_H

#include <ctype.h>
#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <string.h>

#include "../utils/utils.h"

/**
 * \brief          The structure for the input fields in the paradox form.
 *
 */
struct FormInputField {
    char* label;
    unsigned short default_value;
    int max_length;
};

/**
 * \brief          The y-padding of the form relative to the parent window.
 *
 */
#define BH_FORM_Y_PADDING             2
/**
 * \brief          The x-padding of the form relative to the parent window.
 *
 */
#define BH_FORM_X_PADDING             2

/**
 * \brief          The padding between the field and the bracket.
 */
#define BH_FORM_FIELD_BRACKET_PADDING 2

unsigned short calculate_longest_max_length(const struct FormInputField const form_fields[],
                                            uint8_t form_fields_len, bool padding);

FIELD* create_button_field(const char* label, unsigned short frow, unsigned short fcol);
void update_button_field_is_running(FIELD* button_field, const char* label,
                                    const char* running_label, bool is_running);

int calculate_form_max_value(int length);

void update_field_highlighting(FORM* current_form, unsigned short form_field_count,
                               unsigned short form_button_indexes[],
                               unsigned short form_button_indexes_len);

void display_field_error(WINDOW* sub_win, FIELD* field, int field_index,
                         unsigned short max_label_length, unsigned short max_field_length,
                         int max_field_value, bool y_padding);

void clear_field_error(WINDOW* sub_win, int field_index, unsigned short max_label_length,
                       unsigned short max_field_length);

#endif