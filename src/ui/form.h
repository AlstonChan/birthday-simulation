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
#include <form.h>
#include <math.h>
#include <ncurses.h>
#include <string.h>

#include "../utils/utils.h"

/**
 * \brief          Input field configuration
 *
 */
struct FormInputField {
    char* label;                  ///< The label for the form input field
    unsigned short default_value; ///< Default value to set in the buffer on form init
    unsigned int
        max_length; ///< The maximum length the of character of the input field. The actual length of the input field would be N+1 to accomodate the text cursor
};

/**
 * \brief          Button configuration
 */
struct FormButton {
    char*
        label; ///< The value for the button set on init for the field, as readonly to simulate a button
    char*
        loading_label; ///< The value to show when a process is ongoing and to make the button unavailable
    unsigned int
        action_id; // the action id to identify what action should be taken. This value depends on the usage context
};

/**
 * \brief          Runtime tracking structure for form fields. This only
 *                 tracks the current length of one field value that is 
 *                 being edited/active.  
 */
typedef struct FieldTracker {
    FIELD*
        field; ///< the form field this struct is tracking. This should be a writable input field and not a readonly (button) field
    unsigned int current_length;  ///< The current length of the input field the user sees
    unsigned int cursor_position; ///< The actual position of the text cursor the user sees
    unsigned int
        max_length; ///< The max length of character this input field can accept. This should be one character smaller than the input form field X length
    unsigned int field_index; // Index in the metadata array
} field_tracker_t;

/**
 * \brief          Form manager structure
 */
typedef struct FormManager {
    FIELD** fields;            // Combined array for ncurses (inputs + buttons + NULL)
    field_tracker_t* trackers; ///< Tracker struct for writable input field (inputs only)

    int input_count;  ///< The number of input field, not including NULL
    int button_count; ///< The number of button field, not including NULL
    int total_field_count; ///< The number of input + button field, value MUST be input_count + button_count

    unsigned int
        max_label_length; ///< The maximum length of the writable input label of the entire form
    unsigned int
        max_field_length; ///< The maximum length of the writable input field of the entire form

    FORM* form;      ///< The ncurses form that all fields attached to
    WINDOW* sub_win; ///< The sub window that the form rendered at
} form_manager_t;

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

/********************** UTILITY FUNCTIONS **********************/

unsigned short calculate_longest_max_length(const struct FormInputField const form_fields[],
                                            uint8_t form_fields_len, bool padding);
int calculate_form_max_value(int length);

/********************** BUTTON FUNCTIONS ***********************/

FIELD* create_button_field(const char* label, unsigned short frow, unsigned short fcol);
void update_button_field_is_running(FIELD* button_field, const char* label,
                                    const char* running_label, bool is_running);
int get_button_field_indices(int len_a, int len_b, unsigned short** button_indices);
bool is_field_button(form_manager_t* manager, int field_index);

/******************** FORM FIELD FUNCTIONS *********************/

void update_field_highlighting(form_manager_t* manager);
bool validate_field_and_display(form_manager_t* manager);
bool display_field_error(form_manager_t* manager, FIELD* field, unsigned int max_length,
                         bool y_padding);
void clear_field_error(form_manager_t* manager, FIELD* field);

/************************ FORM MANAGERS ************************/

form_manager_t* create_form_manager(const struct FormInputField const input_metadata[],
                                    unsigned short input_metadata_len,
                                    const struct FormButton const button_metadata[],
                                    unsigned short button_metadata_len);
field_tracker_t* find_field_tracker(form_manager_t* manager, FIELD* field);
bool field_has_space_for_char(form_manager_t* manager, FIELD* field);
void increment_field_length(form_manager_t* manager, FIELD* field);
void decrement_field_length(form_manager_t* manager, FIELD* field);
void reset_field_length(form_manager_t* manager, FIELD* field);
void increment_cursor_position(form_manager_t* manager, FIELD* field);
void decrement_cursor_position(form_manager_t* manager, FIELD* field);
void reset_cursor_position(form_manager_t* manager, FIELD* field);
unsigned int get_cursor_position(form_manager_t* manager, FIELD* field);
bool cursor_can_move_right(form_manager_t* manager, FIELD* field);
bool cursor_can_move_left(form_manager_t* manager, FIELD* field);
unsigned short get_field_length_on_screen(form_manager_t* manager, FIELD* field);
int get_field_current_length(form_manager_t* manager, FIELD* field);
void on_field_change(form_manager_t* manager, FIELD* old_field, FIELD* new_field);
void free_form_manager(form_manager_t* manager);
#endif