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