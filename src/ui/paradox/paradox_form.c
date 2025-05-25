#include <ctype.h>
#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "paradox_form.h"

/**
 * @brief The button text of the paradox form to run the simulation.
 *
 */
const char const *paradox_form_button_text = "[ Run Simulation ]";

/**
 * @brief The structure for the input fields in the paradox form.
 *
 */
const struct ParadoxInputField paradox_fields[] = {{"Domain Size (days)", 365, 5},
                                                   {"Sample Count (people)", 23, 10},
                                                   {"Simulation Runs", 1000, 5}};

/**
 * @brief The number of fields in the paradox form.
 *
 */
const unsigned short paradox_fields_len = ARRAY_SIZE(paradox_fields);

/**
 * @brief The longest field label length in the paradox form.
 *
 */
unsigned short max_label_length = 0;

/**
 * @brief Calculates the longest max_length from the paradox_fields array.
 *
 * @return unsigned short The longest max_length value.
 */
static unsigned short calculate_longest_max_length() {
  unsigned short longest = 0;
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    if (paradox_fields[i].max_length > longest) {
      longest = paradox_fields[i].max_length;
    }
  }
  return longest;
}

static FIELD **paradox_field = NULL;
static FORM *paradox_form = NULL;
static WINDOW *paradox_form_sub_win = NULL;

FIELD *paradox_field_get(int index) { return paradox_field[index]; }
FIELD **paradox_field_get_all() { return paradox_field; }
FORM *paradox_form_get() { return paradox_form; }
WINDOW *paradox_form_sub_win_get() { return paradox_form_sub_win; }

/**
 * @brief Updates the highlighting of the paradox form fields based on the current field.
 *
 */
void update_field_highlighting() {
  if (paradox_form == NULL)
    return;

  FIELD **fields = form_fields(paradox_form);
  FIELD *current = current_field(paradox_form);
  int current_index = field_index(current);

  // Show or hide cursor based on whether we're on the button
  if (current_index == paradox_fields_len) {
    curs_set(0); // Hide cursor on button
  } else {
    curs_set(1); // Show cursor on input fields
  }

  for (unsigned short i = 0; i < paradox_fields_len + 1; ++i) {
    if (fields[i] == current) {
      if (i == paradox_fields_len) {
        // Button selected - invert colors
        set_field_back(fields[i], A_REVERSE | COLOR_PAIR(BUTTON_COLOR_PAIR));
      } else {
        // Input field selected
        set_field_back(fields[i], A_REVERSE);
      }
    } else {
      if (i == paradox_fields_len) {
        // Button not selected - normal button colors
        set_field_back(fields[i], A_NORMAL | COLOR_PAIR(BUTTON_COLOR_PAIR));
      } else {
        // Input field not selected
        set_field_back(fields[i], A_NORMAL);
      }
    }
  }

  form_driver(paradox_form, REQ_VALIDATION); // Force form refresh
}

/**
 * @brief Calculates the maximum value based on the length of the field.
 * @example If max_length is 3, max_value is 999
 *
 * @param length The length of the field.
 * @return int The maximum value based on the length of the field.
 */
static int calculate_max_value(int length) {
  if (length <= 0)
    return 0; // Or handle as an error

  // pow(10, length) - 1 gives the largest number with 'length' digits
  // Example: pow(10, 3) - 1 = 1000 - 1 = 999
  return (int)pow(10, length) - 1;
}

/**
 * @brief Displays an error message for a field in the paradox form.
 *
 * @param field The field to display the error message for.
 * @param field_index The index of the field.
 */
void display_field_error(FIELD *field, int field_index) {
  char *buffer = field_buffer(field, 0);
  int value;

  WINDOW *win = paradox_form_sub_win;
  int y_pos = field_index + FORM_Y_PADDING;

  // Clear any previous error message first
  mvwprintw(
      win, y_pos, max_label_length + calculate_longest_max_length() + 6, "                    ");

  // Trim trailing spaces from buffer
  char *end = buffer + strlen(buffer) - 1;
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
    wattron(win, COLOR_PAIR(ERROR_COLOR_PAIR));
    mvwprintw(
        win, y_pos, max_label_length + calculate_longest_max_length() + 6, "Must be a number");
    wattroff(win, COLOR_PAIR(ERROR_COLOR_PAIR));
    return;
  }

  // Get the maximum allowed value based on the field's max length
  int min = 1;
  int max = calculate_max_value(paradox_fields[field_index].max_length);

  if (value < min || value > max) {
    wattron(win, COLOR_PAIR(ERROR_COLOR_PAIR));
    mvwprintw(win,
              y_pos,
              max_label_length + calculate_longest_max_length() + 6,
              "Range: %d-%d",
              min,
              max);
    wattroff(win, COLOR_PAIR(ERROR_COLOR_PAIR));
  }
}

/**
 * @brief Initializes the paradox form with the given window.
 * If no window is provided, it will return early without doing anything.
 *
 * @param win The window to display the form in. This should ideally be
 * the content window.
 */
void paradox_form_init(WINDOW *win) {
  if (paradox_form)
    return; // Already initialized

  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  // Initialize color pairs
  init_pair(ERROR_COLOR_PAIR, COLOR_RED, COLOR_BLACK);
  init_pair(BUTTON_COLOR_PAIR, COLOR_GREEN, COLOR_BLACK);

  // Allocate memory for the form fields
  paradox_field = (FIELD **)calloc((size_t)(paradox_fields_len + 2), sizeof(FIELD *));

  // Find the longest label length for the fields
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    unsigned short label_length = strlen(paradox_fields[i].label);
    if (label_length > max_label_length)
      max_label_length = label_length;
  }

  // Get the longest max_length from the fields
  unsigned short max_field_length = calculate_longest_max_length();

  // Create form fields for each paradox field
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    paradox_field[i] = new_field(1,                    // Field height
                                 max_field_length,     // Field width
                                 i + FORM_Y_PADDING,   // Field y-position
                                 max_label_length + 4, // Field x-position
                                 0,                    // number of offscreen rows
                                 0                     // number of additional working buffers
    );

    // Convert the default value to string
    char *string_buffer = (char *)malloc(sizeof(char) * paradox_fields[i].max_length + 1);
    snprintf(string_buffer, paradox_fields[i].max_length, "%hu", paradox_fields[i].default_value);

    // Make the field visible and editable
    field_opts_off(paradox_field[i], O_AUTOSKIP | O_STATIC);
    set_field_back(paradox_field[i], A_NORMAL); // Set normal background for all fields initially
    set_field_buffer(paradox_field[i], 0, string_buffer); // Set the default value for the field

    // Set maximum field length
    set_max_field(paradox_field[i], max_field_length);

    free(string_buffer);

    // Set the field type to numeric
    int max_value = calculate_max_value(paradox_fields[i].max_length);
    set_field_type(paradox_field[i], TYPE_INTEGER, 0, (long)1, (long)max_value);
  }

  // Create the submit button field
  unsigned short button_width = strlen(paradox_form_button_text);
  paradox_field[paradox_fields_len] =
      new_field(1, button_width, paradox_fields_len + 3, FORM_X_PADDING, 0, 0);
  set_field_buffer(paradox_field[paradox_fields_len], 0, paradox_form_button_text);
  field_opts_off(paradox_field[paradox_fields_len], O_EDIT); // Make the button field non-editable
  set_field_back(paradox_field[paradox_fields_len], A_BOLD | COLOR_PAIR(BUTTON_COLOR_PAIR));

  // Add NULL terminator after the button
  paradox_field[paradox_fields_len + 1] = NULL;

  // Create a sub-window for the form with extra space for the button
  paradox_form_sub_win = derwin(win, paradox_fields_len + 5, COLS - 4, 1, 1);
  keypad(paradox_form_sub_win, TRUE);

  // Create the form
  paradox_form = new_form(paradox_field);
  set_form_win(paradox_form, win);
  set_form_sub(paradox_form, paradox_form_sub_win);
  post_form(paradox_form);

  set_current_field(paradox_form, paradox_field[0]);
  update_field_highlighting(paradox_form);
}

/**
 * @brief Renders the paradox form in the given window.
 * If no window is provided, it will return early without doing anything.
 *
 * @param win The window to render the form in. This should ideally be
 * the content window.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 */
FORM *paradox_form_render(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    return NULL; // If no window is provided, do nothing

  if (paradox_form == NULL)
    paradox_form_init(win); // Initialize the form if not already done

  // Set the label for the field
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    mvwprintw(paradox_form_sub_win, i + FORM_Y_PADDING, FORM_X_PADDING, paradox_fields[i].label);
    mvwprintw(paradox_form_sub_win, i + FORM_Y_PADDING, max_label_length + FORM_X_PADDING, ": ");
  }

  form_driver(paradox_form, REQ_END_LINE);

  wrefresh(win);

  return paradox_form;
}

/**
 * @brief Destroys the paradox form and frees the memory allocated for it.
 *
 */
void paradox_form_destroy() {
  if (!paradox_form)
    return;

  unpost_form(paradox_form);
  free_form(paradox_form);

  // Free the fields
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    free_field(paradox_field[i]);
  }
  free(paradox_field[paradox_fields_len]);     // Free the NULL terminator
  free(paradox_field[paradox_fields_len + 1]); // Free the button field

  paradox_field = NULL;
  paradox_form = NULL;
  paradox_form_sub_win = NULL;
}

void paradox_form_clear_error_message(int field_index) {
  mvwprintw(paradox_form_sub_win,
            field_index + FORM_Y_PADDING,
            max_label_length + calculate_longest_max_length() + 6,
            "                    ");
}

bool paradox_form_validate_all_fields(WINDOW *win) {
  bool all_valid = true;
  for (int i = 0; i < paradox_fields_len; i++) {
    set_current_field(paradox_form, paradox_field_get(i));
    int result = form_driver(paradox_form, REQ_VALIDATION);

    if (result == E_INVALID_FIELD) {
      all_valid = false;
      display_field_error(paradox_field_get(i), i);
    }
  }

  if (all_valid) {
    // All fields are valid, display the values
    mvwprintw(win, 15, 2, "Running simulation with the following parameters:");
    for (int i = 0; i < paradox_fields_len; i++) {
      char *value = field_buffer(paradox_field_get(i), 0);
      mvwprintw(win, 15 + 1 + i, 2, "%s: %s", paradox_fields[i].label, value);
    }
    wrefresh(win);

    set_current_field(paradox_form, paradox_field_get(paradox_fields_len));
  } else {
    set_current_field(paradox_form, paradox_field_get(0));
  }

  return all_valid;
}

void paradox_form_handle_input(WINDOW *win, int ch) {
  FIELD *current = current_field(paradox_form);
  int current_index = field_index(current);

  switch (ch) {
  case KEY_DOWN: {
    int result = form_driver(paradox_form, REQ_VALIDATION);
    form_driver(paradox_form, REQ_NEXT_FIELD);
    form_driver(paradox_form, REQ_END_LINE);

    current = current_field(paradox_form);
    current_index = field_index(current);

    if (result == E_INVALID_FIELD) {
      display_field_error(current, current_index);
    } else {
      paradox_form_clear_error_message(current_index);
    }

    update_field_highlighting();

    if (current_index < paradox_fields_len) {
      pos_form_cursor(paradox_form);
    } else {
      set_field_buffer(paradox_field_get(paradox_fields_len), 0, paradox_form_button_text);
      pos_form_cursor(paradox_form);
    }
  } break;

  case KEY_UP: {
    int result = form_driver(paradox_form, REQ_VALIDATION);
    form_driver(paradox_form, REQ_PREV_FIELD);
    form_driver(paradox_form, REQ_END_LINE);

    current = current_field(paradox_form);
    current_index = field_index(current);

    if (result == E_INVALID_FIELD) {
      display_field_error(current, current_index);
    } else {
      paradox_form_clear_error_message(current_index);
    }

    update_field_highlighting();

    if (current_index < paradox_fields_len) {
      pos_form_cursor(paradox_form);
    } else {
      set_field_buffer(paradox_field_get(paradox_fields_len), 0, paradox_form_button_text);
      pos_form_cursor(paradox_form);
    }
  } break;

  case KEY_LEFT:
    if (current_index < paradox_fields_len) {
      form_driver(paradox_form, REQ_PREV_CHAR);
    }
    break;

  case KEY_RIGHT:
    if (current_index < paradox_fields_len) {
      form_driver(paradox_form, REQ_NEXT_CHAR);
    }
    break;

  case KEY_BACKSPACE:
  case 127:
    if (current_index < paradox_fields_len) {
      // Get field buffer
      char *buffer = field_buffer(current, 0);

      // Find the first space character which indicates end of content
      int content_length = strcspn(buffer, " ");

      // If we have content to delete, allow backspace
      if (content_length > 0) {
        form_driver(paradox_form, REQ_DEL_PREV);
      }
    }
    break;

  case KEY_DC:
    if (current_index < paradox_fields_len) {
      form_driver(paradox_form, REQ_DEL_CHAR);
    }
    break;

  case '\n': {
    int result = form_driver(paradox_form, REQ_VALIDATION);

    if (result == E_INVALID_FIELD) {
      display_field_error(current, current_index);
    } else if (current_index == paradox_fields_len) {
      paradox_form_validate_all_fields(win);
    }
  } break;

  default:
    if (current_index < paradox_fields_len && isdigit(ch)) {
      form_driver(paradox_form, ch);
      form_driver(paradox_form, REQ_END_FIELD);
    }
    break;
  }
  wrefresh(win);
}