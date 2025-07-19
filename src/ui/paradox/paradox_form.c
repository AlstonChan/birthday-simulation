#include <ctype.h>
#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/paradox_math.h"
#include "../../utils/utils.h"
#include "paradox_form.h"

const char const *form_submit_button_text = "[ Run Simulation ]";

const struct FormInputField paradox_form_field_metadata[] = {
    {"Domain Size (days)", 365, 5}, {"Sample Count (people)", 23, 9}, {"Simulation Runs", 1000, 5}};
const unsigned short paradox_form_field_metadata_len = ARRAY_SIZE(paradox_form_field_metadata);

/**
 * @brief The longest field label length in the paradox form.
 *
 */
static unsigned short max_label_length = 0;

static FIELD **paradox_form_field = NULL;
static FORM *paradox_form = NULL;
static WINDOW *paradox_form_sub_win = NULL;

FIELD *paradox_form_field_get(int index) { return paradox_form_field[index]; }
FIELD **paradox_form_field_get_all() { return paradox_form_field; }
FORM *paradox_form_get() { return paradox_form; }
WINDOW *paradox_form_sub_win_get() { return paradox_form_sub_win; }

void paradox_form_init(WINDOW *win, int max_y, int max_x) {
  if (paradox_form)
    return; // Already initialized

  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  // Allocate memory for the form fields
  paradox_form_field =
      (FIELD **)calloc((size_t)(paradox_form_field_metadata_len + 2), sizeof(FIELD *));

  // Find the longest label length for the fields
  for (unsigned short i = 0; i < paradox_form_field_metadata_len; ++i) {
    unsigned short label_length = strlen(paradox_form_field_metadata[i].label);
    if (label_length > max_label_length)
      max_label_length = label_length;
  }

  // Get the longest max_length from the fields
  unsigned short max_field_length = calculate_longest_max_length(
      paradox_form_field_metadata, paradox_form_field_metadata_len, false);

  // Create form fields for each paradox field
  for (unsigned short i = 0; i < paradox_form_field_metadata_len; ++i) {
    paradox_form_field[i] =
        new_field(1,                     // Field height
                  max_field_length + 1,  // Field width
                  i + BH_FORM_Y_PADDING, // Field y-position
                  BH_FORM_X_PADDING + BH_FORM_FIELD_BRACKET_PADDING + max_label_length +
                      BH_FORM_FIELD_BRACKET_PADDING, // Field x-position
                  0,                                 // number of offscreen rows
                  0                                  // number of additional working buffers
        );

    // Convert the default value to string
    char *string_buffer =
        (char *)malloc(sizeof(char) * paradox_form_field_metadata[i].max_length + 1);
    snprintf(string_buffer,
             paradox_form_field_metadata[i].max_length + 1,
             "%hu",
             paradox_form_field_metadata[i].default_value);

    // Make the field visible and editable
    field_opts_on(paradox_form_field[i], O_STATIC);    // Keep field static size
    field_opts_off(paradox_form_field[i], O_AUTOSKIP); // Don't auto skip to next field
    set_field_back(paradox_form_field[i],
                   A_NORMAL); // Set normal background for all fields initially
    set_field_buffer(
        paradox_form_field[i], 0, string_buffer);        // Set the default value for the field
    set_field_just(paradox_form_field[i], JUSTIFY_LEFT); // Left justify the content

    free(string_buffer);

    // Set maximum field length
    set_max_field(paradox_form_field[i], max_field_length);

    // Set the field type to numeric
    int max_value = calculate_form_max_value(paradox_form_field_metadata[i].max_length);
    set_field_type(paradox_form_field[i], TYPE_INTEGER, 0, (long)1, (long)max_value);
  }

  // Create the submit button field
  paradox_form_field[paradox_form_field_metadata_len] =
      create_button_field(form_submit_button_text, // Button label
                          paradox_form_field_metadata_len + BH_FORM_Y_PADDING + 1,
                          BH_FORM_X_PADDING);

  // Add NULL terminator after the button
  paradox_form_field[paradox_form_field_metadata_len + 1] = NULL;

  // Create the form
  paradox_form = new_form(paradox_form_field);
  paradox_form_create_sub_win(win, max_y, max_x);
}

FORM *paradox_form_render(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    return NULL; // If no window is provided, do nothing

  if (paradox_form == NULL)
    paradox_form_init(win, max_y, max_x); // Initialize the form if not already done

  // Set the label for the field
  for (unsigned short i = 0; i < paradox_form_field_metadata_len; ++i) {
    mvwprintw(paradox_form_sub_win,
              i + BH_FORM_Y_PADDING,
              BH_FORM_X_PADDING,
              paradox_form_field_metadata[i].label);
    mvwprintw(
        paradox_form_sub_win, i + BH_FORM_Y_PADDING, BH_FORM_X_PADDING + max_label_length, ": [");
    mvwprintw(paradox_form_sub_win,
              i + BH_FORM_Y_PADDING,
              BH_FORM_X_PADDING + max_label_length + BH_FORM_FIELD_BRACKET_PADDING + 1 +
                  calculate_longest_max_length(
                      paradox_form_field_metadata, paradox_form_field_metadata_len, true) +
                  BH_FORM_FIELD_BRACKET_PADDING,
              "]");
  }

  set_current_field(paradox_form, paradox_form_field[0]);
  update_field_highlighting(paradox_form,
                            paradox_form_field_metadata_len + 1,
                            (unsigned short[]){paradox_form_field_metadata_len},
                            1);
  form_driver(paradox_form, REQ_END_LINE);

  wrefresh(win);

  return paradox_form;
}

void paradox_form_create_sub_win(WINDOW *win, int max_y, int max_x) {
  if (paradox_form_sub_win) {
    delwin(paradox_form_sub_win);
    paradox_form_sub_win = NULL;
  }

  // Create a sub-window for the form with extra space for the button
  paradox_form_sub_win = derwin(win, paradox_form_field_metadata_len + 5, max_x - 4, 1, 1);
  keypad(paradox_form_sub_win, TRUE);

  set_form_win(paradox_form, win);
  set_form_sub(paradox_form, paradox_form_sub_win);

  unpost_form(paradox_form); // Safeguard against state issues
  post_form(paradox_form);
}

void paradox_form_erase() {
  if (!paradox_form)
    return;

  unpost_form(paradox_form);
}

void paradox_form_restore(WINDOW *win, int max_y, int max_x) {
  if (!paradox_form)
    return;

  // recreate the sub win
  paradox_form_create_sub_win(win, max_y, max_x);

  // Force re-render of field labels
  paradox_form_render(win, max_y, max_x);

  // Manually restore field buffers
  for (int i = 0; paradox_form_field[i] != NULL; ++i) {
    const char *buf = field_buffer(paradox_form_field[i], 0);
    set_field_buffer(paradox_form_field[i], 0, buf); // Force internal repaint
  }

  // Step 6: Force redraw current field again
  set_current_field(paradox_form, paradox_form_field[0]);
  form_driver(paradox_form, REQ_FIRST_FIELD);

  // Step 7: Final refresh
  wrefresh(paradox_form_sub_win);

  // post_form(paradox_form);
  // paradox_form_render(win, max_y, max_x);
  // wrefresh(paradox_form_sub_win);
}

void paradox_form_destroy() {
  if (!paradox_form)
    return;

  unpost_form(paradox_form);
  free_form(paradox_form);

  // Free the fields
  for (unsigned short i = 0; i < paradox_form_field_metadata_len; ++i) {
    free_field(paradox_form_field[i]);
  }
  free_field(paradox_form_field[paradox_form_field_metadata_len]); // Free the NULL terminator
  free(paradox_form_field[paradox_form_field_metadata_len + 1]);   // Free the button field

  paradox_form_field = NULL;
  paradox_form = NULL;
  paradox_form_sub_win = NULL;
}

bool paradox_form_validate_all_fields(WINDOW *win) {
  bool all_valid = true;
  for (int i = 0; i < paradox_form_field_metadata_len; i++) {
    set_current_field(paradox_form, paradox_form_field_get(i));
    int result = form_driver(paradox_form, REQ_VALIDATION);
    unsigned short longest_max_length_pad = calculate_longest_max_length(
        paradox_form_field_metadata, paradox_form_field_metadata_len, true);

    if (result == E_INVALID_FIELD) {
      all_valid = false;
      display_field_error(paradox_form_sub_win,
                          paradox_form_field_get(i),
                          i,
                          max_label_length,
                          longest_max_length_pad,
                          calculate_form_max_value(paradox_form_field_metadata[i].max_length),
                          true);
    }
  }

  if (all_valid) {
    int form_win_x, form_win_y;
    getmaxyx(paradox_form_sub_win, form_win_y, form_win_x);

    int domain_size = atoi(field_buffer(paradox_form_field_get(0), 0));
    int sample_count = atoi(field_buffer(paradox_form_field_get(1), 0));
    int simulation_runs = atoi(field_buffer(paradox_form_field_get(2), 0));

    // Calculate the estimated chance of a collision for a single simulation run
    double collision_probability =
        calculate_birthday_collision_probability(domain_size, sample_count);

    // Display the estimated chance of a collision
    mvwprintw(
        win, form_win_y + 2, BH_FORM_X_PADDING + 1, "Estimated chance of a collision:       ");
    mvwprintw(win,
              form_win_y + 2,
              BH_FORM_X_PADDING + 1,
              "Estimated chance of a collision: %.2f%%",
              collision_probability * 100);

    // Display the simulated runs results
    double simulated_runs_results =
        simulate_birthday_collision(domain_size, sample_count, simulation_runs);

    // Display the simulated runs results
    mvwprintw(win, form_win_y + 3, BH_FORM_X_PADDING + 1, "Simulated runs results:       ");
    mvwprintw(win,
              form_win_y + 3,
              BH_FORM_X_PADDING + 1,
              "Simulated runs results: %.2f%%",
              simulated_runs_results);

    wrefresh(win);

    set_current_field(paradox_form, paradox_form_field_get(paradox_form_field_metadata_len));
  } else {
    set_current_field(paradox_form, paradox_form_field_get(0));
  }

  return all_valid;
}

void paradox_form_handle_input(WINDOW *win, int ch) {
  FIELD *current = current_field(paradox_form);
  int current_index = field_index(current);
  unsigned short longest_max_length_pad = calculate_longest_max_length(
      paradox_form_field_metadata, paradox_form_field_metadata_len, true);

  switch (ch) {
  case KEY_UP:
  case KEY_DOWN: {
    int result = form_driver(paradox_form, REQ_VALIDATION);
    if (ch == KEY_DOWN) {
      form_driver(paradox_form, REQ_NEXT_FIELD);
    } else {
      form_driver(paradox_form, REQ_PREV_FIELD);
    }
    form_driver(paradox_form, REQ_END_LINE);

    current = current_field(paradox_form);
    current_index = field_index(current);

    if (result == E_INVALID_FIELD) {
      display_field_error(
          paradox_form_sub_win,
          current,
          current_index,
          max_label_length,
          longest_max_length_pad,
          calculate_form_max_value(paradox_form_field_metadata[current_index].max_length),
          true);

    } else {
      clear_field_error(
          paradox_form_sub_win, current_index, max_label_length, longest_max_length_pad);
    }

    update_field_highlighting(paradox_form,
                              paradox_form_field_metadata_len + 1,
                              (unsigned short[]){paradox_form_field_metadata_len},
                              1);

    if (current_index < paradox_form_field_metadata_len) {
      pos_form_cursor(paradox_form);
    } else {
      set_field_buffer(
          paradox_form_field_get(paradox_form_field_metadata_len), 0, form_submit_button_text);
      pos_form_cursor(paradox_form);
    }
  } break;

  case KEY_LEFT:
    if (current_index < paradox_form_field_metadata_len) {
      form_driver(paradox_form, REQ_PREV_CHAR);
    }
    break;
  case KEY_RIGHT:
    if (current_index < paradox_form_field_metadata_len) {
      form_driver(paradox_form, REQ_NEXT_CHAR);
    }
    break;

  case KEY_BACKSPACE:
  case 127:
    form_driver(paradox_form, REQ_DEL_PREV);
    break;
  case KEY_DC:
    if (current_index < paradox_form_field_metadata_len) {
      form_driver(paradox_form, REQ_DEL_CHAR);
    }
    break;

  case '\n': {
    int result = form_driver(paradox_form, REQ_VALIDATION);

    if (result == E_INVALID_FIELD) {
      display_field_error(
          paradox_form_sub_win,
          current,
          current_index,
          max_label_length,
          longest_max_length_pad,
          calculate_form_max_value(paradox_form_field_metadata[current_index].max_length),
          true);
    } else if (current_index == paradox_form_field_metadata_len) {
      paradox_form_validate_all_fields(win);
    }
  } break;

  default: {
    if (current_index < paradox_form_field_metadata_len && isdigit(ch)) {
      form_driver(paradox_form, ch);
    }
  } break;
  }
}