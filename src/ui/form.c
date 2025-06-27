#include <math.h>
#include <ncurses/form.h>
#include <string.h>

#include "../utils/utils.h"
#include "form.h"

unsigned short calculate_longest_max_length(const struct FormInputField const form_fields[],
                                            uint8_t form_fields_len, bool padding) {
  unsigned short longest = 0;

  for (unsigned short i = 0; i < form_fields_len; ++i) {
    if (form_fields[i].max_length > longest) {
      longest = form_fields[i].max_length;
    }
  }

  if (padding)
    longest++;

  return longest;
}

FIELD *create_button_field(const char *label, unsigned short frow, unsigned short fcol) {
  unsigned short button_width = strlen(label);

  FIELD *button_field = new_field(1,            // Field height
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

int calculate_form_max_value(int length) {
  if (length <= 0)
    return 0; // Or handle as an error

  // pow(10, length) - 1 gives the largest number with 'length' digits
  // Example: pow(10, 3) - 1 = 1000 - 1 = 999
  return (int)pow(10, length) - 1;
}

void update_field_highlighting(FORM *current_form, unsigned short form_field_count,
                               unsigned short form_button_indexes[],
                               unsigned short form_button_indexes_len) {
  if (current_form == NULL || form_field_count == 0 || form_button_indexes == NULL)
    return;

  FIELD **fields = form_fields(current_form);
  FIELD *current = current_field(current_form);
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