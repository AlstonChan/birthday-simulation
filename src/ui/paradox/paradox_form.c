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

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/

/**
 * @brief Create a sub window from the parent window for the form
 *
 * @param win The window that will contain the created subwin for the form. This
 * should ideally be the content win
 * @param max_y The maximum height of the screen space that can be rendered
 * @param max_x The maximum width of the screen space that can be rendered
 */
static void paradox_form_create_sub_win(WINDOW *win, int max_y, int max_x) {
  if (win == NULL) {
    render_full_page_error_exit(
        stdscr, 0, 0, "The window passed to paradox_form_create_sub_win is null");
  }

  if (paradox_form_sub_win) {
    delwin(paradox_form_sub_win);
    paradox_form_sub_win = NULL;
  }

  const int sub_win_rows_count = BH_FORM_Y_PADDING + // One border row, one spacing row
                                 paradox_form_field_metadata_len + // Number of input
                                 1 +                               // One spacing row
                                 1 +                               // Button Row
                                 BH_FORM_Y_PADDING; // One border row, one spacing row

  // Create a sub-window for the form with extra space for the button
  paradox_form_sub_win = derwin(win, sub_win_rows_count, max_x - 4, 1, 1);
  keypad(paradox_form_sub_win, TRUE);

  set_form_win(paradox_form, win);
  set_form_sub(paradox_form, paradox_form_sub_win);

  unpost_form(paradox_form); // Safeguard against state issues
  post_form(paradox_form);
}

/**
 * @brief Take the value from the form field as arguments for calculating the
 * collision and simulation. The result will be stored back to the arguments
 * provided to the function.
 *
 * @param collision_probability A reference of the value of the probability of
 * hash collision of this result
 * @param simulated_runs_results A reference of the value of the actual collision
 * occur in simulations
 */
static void run_simulation_from_input(double *collision_probability,
                                      double *simulated_runs_results) {
  int domain_size = atoi(field_buffer(paradox_form_field_get(0), 0));
  int sample_count = atoi(field_buffer(paradox_form_field_get(1), 0));
  int simulation_runs = atoi(field_buffer(paradox_form_field_get(2), 0));

  *collision_probability = calculate_birthday_collision_probability(domain_size, sample_count);
  *simulated_runs_results = simulate_birthday_collision(domain_size, sample_count, simulation_runs);
}

/**
 * @brief Render the simulation result given the value in the arguments
 *
 * @param win The window that it will render the content on, which also contain
 * the sub win that holds the form. It will assume that the subwin is at the top
 * of the window and will render the content {BH_FORM_Y_PADDING} under it
 * @param collision_probability The result of the collision probability to render
 * @param simulated_runs_results The result of the simulations to render
 */
static void render_simulation_result(WINDOW *win, double collision_probability,
                                     double simulated_runs_results) {
  if (win == NULL) {
    render_full_page_error_exit(
        stdscr, 0, 0, "The window passed to render_simulation_result is null");
  }

  int form_win_x, form_win_y;
  getmaxyx(paradox_form_sub_win, form_win_y, form_win_x);

  uint8_t starting_rows = form_win_y + BH_FORM_Y_PADDING;
  uint8_t starting_cols = BH_FORM_X_PADDING + 1;

  mvwprintw(win, starting_rows, starting_cols, "Estimated chance of a collision:       ");
  mvwprintw(win,
            starting_rows,
            starting_cols,
            "Estimated chance of a collision: %.2f%%",
            collision_probability * 100);

  mvwprintw(win, starting_rows + 1, starting_cols, "Simulated runs results:       ");
  mvwprintw(win,
            starting_rows + 1,
            starting_cols,
            "Simulated runs results: %.2f%%",
            simulated_runs_results);

  wrefresh(win);
}

/**
 * @brief Loop over all field and validate the field. Error message will
 * be displayed at the side of the field if any
 *
 * @return true No error found, all field is valid
 * @return false One or more input is invalid
 */
static bool paradox_form_validate_all_fields() {
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
    set_current_field(paradox_form, paradox_form_field_get(paradox_form_field_metadata_len));
  } else {
    set_current_field(paradox_form, paradox_form_field_get(0));
  }

  return all_valid;
}

/****************************************************************
                       EXTERNAL FUNCTION
****************************************************************/

void paradox_form_init(WINDOW *win, int max_y, int max_x) {
  if (win == NULL) {
    render_full_page_error_exit(stdscr, 0, 0, "The window passed to paradox_form_init is null");
  }

  if (paradox_form) {
    render_full_page_error_exit(win,
                                0,
                                0,
                                "The paradox form has already been initialized and another "
                                "attempt to initialize is not permitted");
  }

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
  if (win == NULL) {
    render_full_page_error_exit(stdscr, 0, 0, "The window passed to paradox_form_render is null");
  }

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

void paradox_form_restore(WINDOW *win, int max_y, int max_x, double collision_probability,
                          double simulated_runs_results) {
  if (!paradox_form)
    return;

  if (win == NULL) {
    render_full_page_error_exit(stdscr, 0, 0, "The window passed to paradox_form_restore is null");
  }

  // recreate the sub win
  paradox_form_create_sub_win(win, max_y, max_x);

  // Force re-render of field labels
  paradox_form_render(win, max_y, max_x);

  // Manually restore field buffers
  for (int i = 0; paradox_form_field[i] != NULL; ++i) {
    const char *buf = field_buffer(paradox_form_field[i], 0);
    set_field_buffer(paradox_form_field[i], 0, buf); // Force internal repaint
  }

  // Force redraw current field again
  set_current_field(paradox_form, paradox_form_field[0]);
  form_driver(paradox_form, REQ_FIRST_FIELD);

  if (collision_probability != -1 && simulated_runs_results != -1) {
    render_simulation_result(win, collision_probability, simulated_runs_results);
  }

  wrefresh(paradox_form_sub_win);
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

void paradox_form_handle_input(WINDOW *win, int ch, double *collision_probability,
                               double *simulated_runs_results) {
  if (win == NULL) {
    render_full_page_error_exit(
        stdscr, 0, 0, "The window passed to paradox_form_handle_input is null");
  }

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
      bool all_field_valid = paradox_form_validate_all_fields();
      if (all_field_valid) {
        run_simulation_from_input(collision_probability, simulated_runs_results);
        render_simulation_result(win, *collision_probability, *simulated_runs_results);
      }
    }
  } break;

  default: {
    if (current_index < paradox_form_field_metadata_len && isdigit(ch)) {
      form_driver(paradox_form, ch);
    }
  } break;
  }
}