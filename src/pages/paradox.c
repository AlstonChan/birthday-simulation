#include <ctype.h>
#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "paradox.h"

/**
 * @brief The y-padding of the form.
 *
 */
#define FORM_Y_PADDING 2
#define FORM_X_PADDING 2

#define MAX_FIELD_LENGTH 8
#define ERROR_COLOR_PAIR 2
#define BUTTON_COLOR_PAIR 3

/**
 * @brief The title of the paradox page.
 *
 */
static const char const *paradox_page_title = "[ Birthday Paradox Simulation ]";

/**
 * @brief The button text of the paradox form to run the simulation.
 *
 */
static const char const *paradox_form_button_text = "[ Run Simulation ]";

/**
 * @brief The structure for the input fields in the paradox form.
 *
 */
static const struct ParadoxInputField paradox_fields[] = {{"Domain Size (days)", 365, 5},
                                                          {"Sample Count (people)", 23, 10},
                                                          {"Simulation Runs", 1000, 5}};

/**
 * @brief The number of fields in the paradox form.
 *
 */
static const unsigned short paradox_fields_len = ARRAY_SIZE(paradox_fields);

/**
 * @brief The longest field label length in the paradox form.
 *
 */
static unsigned short max_label_length = 0;

static FIELD **paradox_field = NULL;
static FORM *paradox_form = NULL;
static WINDOW *paradox_form_sub_win = NULL;

/**
 * @brief Updates the highlighting of form fields based on the current field.
 *
 * @param form The form to update field highlighting for.
 */
static void update_field_highlighting(FORM *form) {
  FIELD **fields = form_fields(form);
  FIELD *current = current_field(form);

  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    if (fields[i] == current) {
      set_field_back(fields[i], A_REVERSE); // Highlight current field
    } else {
      set_field_back(fields[i], A_NORMAL); // Remove highlight from other fields
    }
  }

  form_driver(form, REQ_VALIDATION); // Force form refresh
}

// Function to calculate the maximum value based on max_length
// For example, if max_length is 3, max_value is 999
// If max_length is 5, max_value is 99999
int calculate_max_value(int length) {
  if (length <= 0)
    return 0; // Or handle as an error

  // pow(10, length) - 1 gives the largest number with 'length' digits
  // Example: pow(10, 3) - 1 = 1000 - 1 = 999
  return (int)pow(10, length) - 1;
}

static void display_field_error(FIELD *field, int field_index) {
  char *buffer = field_buffer(field, 0);
  int value;

  WINDOW *win = paradox_form_sub_win;
  int y_pos = field_index + FORM_Y_PADDING;

  // Clear any previous error message first
  mvwprintw(win, y_pos, max_label_length + MAX_FIELD_LENGTH + 6, "                    ");

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
    mvwprintw(win, y_pos, max_label_length + MAX_FIELD_LENGTH + 6, "Must be a number");
    wattroff(win, COLOR_PAIR(ERROR_COLOR_PAIR));
    return;
  }

  // Get the maximum allowed value based on the field's max length
  int min = 1;
  int max = calculate_max_value(paradox_fields[field_index].max_length);

  if (value < min || value > max) {
    wattron(win, COLOR_PAIR(ERROR_COLOR_PAIR));
    mvwprintw(win, y_pos, max_label_length + MAX_FIELD_LENGTH + 6, "Range: %d-%d", min, max);
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

  // Create form fields for each paradox field
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    paradox_field[i] = new_field(1,                    // Field height
                                 MAX_FIELD_LENGTH,     // Field width
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
    set_max_field(paradox_field[i], MAX_FIELD_LENGTH);

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

/**
 * @brief Renders the paradox page in the given window.
 * If no window is provided, it will use stdscr.
 *
 * @param win The window to render the paradox page in. If NULL, uses stdscr.
 * @param max_y The maximum y-coordinate of the parent window (stdscr).
 * @param max_x The maximum x-coordinate of the parent window (stdscr).
 */
void render_paradox_page(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  curs_set(1);        // Show the cursor
  nodelay(win, TRUE); // Make getch() non-blocking

  // Clear the window before rendering
  werase(win);

  // Resize the window for the paradox page
  wresize(win, max_y - 6, max_x);

  // Center the paradox page window
  mvwin(win, 4, 0);

  box(win, 0, 0);

  unsigned short title_len = strlen(paradox_page_title);
  mvwprintw(win, 0, (max_x - title_len) / 2, paradox_page_title);

  paradox_form_init(win);                                      // Initialize the paradox form
  FORM *paradox_form = paradox_form_render(win, max_y, max_x); // Render the paradox form

  // Refresh the window to show the paradox page
  wrefresh(win);
  pos_form_cursor(paradox_form); // Position the cursor for the current field

  int ch;
  bool is_button_selected = false;

  /* Loop through to get user requests */
  while ((ch = wgetch(win)) != KEY_F(2)) {
    FIELD *current = current_field(paradox_form);
    int current_index = field_index(current);

    switch (ch) {
    case KEY_DOWN: {
      int result = form_driver(paradox_form, REQ_VALIDATION);
      form_driver(paradox_form, REQ_NEXT_FIELD);
      form_driver(paradox_form, REQ_END_LINE);

      // Get the new current field after moving
      current = current_field(paradox_form);
      current_index = field_index(current);

      // Check and display error for the field we're leaving
      if (result == E_INVALID_FIELD) {
        display_field_error(current, current_index);
      } else {
        // Clear error message when field is valid
        mvwprintw(paradox_form_sub_win,
                  current_index + FORM_Y_PADDING,
                  max_label_length + MAX_FIELD_LENGTH + 6,
                  "                    ");
      }

      update_field_highlighting(paradox_form);

      if (current_index < paradox_fields_len) {
        pos_form_cursor(paradox_form);
      } else {
        set_field_buffer(paradox_field[paradox_fields_len], 0, paradox_form_button_text);
        pos_form_cursor(paradox_form);
      }
    } break;
    case KEY_UP: {
      int result = form_driver(paradox_form, REQ_VALIDATION);
      form_driver(paradox_form, REQ_PREV_FIELD);
      form_driver(paradox_form, REQ_END_LINE);

      // Get the new current field after moving
      current = current_field(paradox_form);
      current_index = field_index(current);

      // Check and display error for the field we're leaving
      if (result == E_INVALID_FIELD) {
        display_field_error(current, current_index);
      } else {
        // Clear error message when field is valid
        mvwprintw(paradox_form_sub_win,
                  current_index + FORM_Y_PADDING,
                  max_label_length + MAX_FIELD_LENGTH + 6,
                  "                    ");
      }

      update_field_highlighting(paradox_form);

      if (current_index < paradox_fields_len) {
        pos_form_cursor(paradox_form);
      } else {
        set_field_buffer(paradox_field[paradox_fields_len], 0, paradox_form_button_text);
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
    case KEY_BACKSPACE: // Backspace key
    case 127:
      if (current_index < paradox_fields_len) {
        form_driver(paradox_form, REQ_DEL_PREV);
      }
      break;
    case KEY_DC: // Delete key
      if (current_index < paradox_fields_len) {
        form_driver(paradox_form, REQ_DEL_CHAR);
      }
      break;
    case '\n': // Enter key
    {
      int result = form_driver(paradox_form, REQ_VALIDATION);

      if (result == E_INVALID_FIELD) {
        display_field_error(current, current_index);
      } else if (current_index == paradox_fields_len) {
        // Button is selected, validate all fields before proceeding
        bool all_valid = true;
        for (int i = 0; i < paradox_fields_len; i++) {
          set_current_field(paradox_form, paradox_field[i]);
          int result = form_driver(paradox_form, REQ_VALIDATION);

          if (result == E_INVALID_FIELD) {
            all_valid = false; // If any field is invalid, set all_valid to false
            display_field_error(paradox_field[i], i); // Display error for the field
          }
        }

        if (all_valid) {
          // All fields are valid, we can proceed
          mvwprintw(win, 15, 2, "Running simulation with the following parameters:");
          for (int i = 0; i < paradox_fields_len; i++) {
            char *value = field_buffer(paradox_field[i], 0);
            mvwprintw(win, 15 + 1 + i, 2, "%s: %s", paradox_fields[i].label, value);
          }
          wrefresh(win);

          set_current_field(paradox_form,
                            paradox_field[paradox_fields_len]); // Set focus back to the button
        } else {
          // Return to the first invalid field
          set_current_field(paradox_form, paradox_field[0]);
        }
      }
    } break;
    default:
      if (current_index < paradox_fields_len && isdigit(ch)) {
        form_driver(paradox_form, ch);
        form_driver(paradox_form, REQ_END_LINE); // Move cursor to end after input
      }
      break;
    }
    wrefresh(win);
  }

  paradox_form_destroy();

  curs_set(0);         // Hide the cursor
  nodelay(win, FALSE); // Make getch() blocking

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);
}