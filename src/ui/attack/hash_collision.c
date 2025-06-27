#include <ctype.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hash_collision.h"
#include "hash_config.h"

#include "../../utils/utils.h"
#include "../form.h"
#include "../layout.h"

static const char *hash_collision_page_title = "[ Hash Collision Demonstration ]";
static const char const *hash_form_submit_button_text = "[ Run Simulation ]";

const struct FormInputField const hash_form_field_metadata[] = {{"Attempts", 1000, 6}};
const unsigned short hash_form_field_metadata_len = ARRAY_SIZE(hash_form_field_metadata);

/**
 * @brief The longest field label length in the hash form.
 *
 */
static unsigned short max_label_length = 0;

static FIELD **hash_form_field = NULL;
static FORM *hash_collision_form = NULL;
static WINDOW *hash_collision_form_sub_win = NULL;

FIELD *hash_collision_form_field_get(int index) { return hash_form_field[index]; }

void render_hash_collision_page(WINDOW *win, int max_y, int max_x, enum hash_function_ids hash_id) {
  curs_set(1);        // Show the cursor
  nodelay(win, TRUE); // Make getch() non-blocking

  // Clear the window before rendering
  werase(win);

  wresize(win, max_y - BH_LAYOUT_PADDING, max_x);
  mvwin(win, 4, 0);
  box(win, 0, 0);

  unsigned short title_len = strlen(hash_collision_page_title);
  mvwprintw(win, 0, (max_x - title_len) / 2, hash_collision_page_title);

  hash_config_t current_hash_function = get_hash_config_item(hash_id);

  // Display the hash function details
  mvwprintw(win, 2, BH_FORM_X_PADDING, "Hash Function       : %s", current_hash_function.label);
  mvwprintw(win, 3, BH_FORM_X_PADDING, "Hash output bits    : %s", current_hash_function.bits);
  mvwprintw(win,
            4,
            BH_FORM_X_PADDING,
            "Estimated Collisions: %s",
            current_hash_function.estimated_collisions);
  mvwprintw(
      win, 5, BH_FORM_X_PADDING, "Space Size          : %s", current_hash_function.space_size);

  // Segment the details and form input fields with a line
  char *separator_line =
      (char *)malloc((max_x - 3) * sizeof(char)); // Allocate space for the separator line

  for (int i = 0; i < max_x - 4; i++) {
    separator_line[i] = '-';
  }
  separator_line[max_x - 4] = '\0';                     // Null-terminate the string
  mvwprintw(win, 7, BH_FORM_X_PADDING, separator_line); // Draw a line below the details

  hash_collision_form_init(win); // Initialize the form fields
  FORM *hash_collision_form = hash_collision_form_render(
      win, max_y - BH_LAYOUT_PADDING, max_x); // Render the form in the window

  bool is_done = false;
  int char_input;
  while ((char_input = wgetch(win)) != KEY_F(2) && !is_done) {
    hash_form_handle_input(win, char_input); // Handle user input for the form
  }

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);

  curs_set(0);         // Hide the cursor
  nodelay(win, FALSE); // Make getch() blocking
}

bool hash_form_validate_all_fields(WINDOW *win) {
  bool all_valid = true;
  unsigned short longest_max_length_pad =
      calculate_longest_max_length(hash_form_field_metadata, hash_form_field_metadata_len, true);

  for (unsigned short i = 0; i < hash_form_field_metadata_len; ++i) {
    FIELD *field = hash_collision_form_field_get(i);
    int result = form_driver(hash_collision_form, REQ_VALIDATION);

    if (result == E_INVALID_FIELD) {
      display_field_error(hash_collision_form_sub_win,
                          field,
                          i,
                          max_label_length,
                          longest_max_length_pad,
                          calculate_form_max_value(hash_form_field_metadata[i].max_length),
                          true);
      all_valid = false;
    } else {
      clear_field_error(hash_collision_form_sub_win, i, max_label_length, longest_max_length_pad);
    }
  }

  if (all_valid) {
    // If all fields are valid, run the simulation
    int attempts = atoi(field_buffer(hash_collision_form_field_get(0), 0));
  }

  return all_valid;
}

void hash_collision_form_init(WINDOW *win) {
  if (hash_collision_form != NULL) {
    return; // If the form is already initialized, do nothing
  }

  if (win == NULL) {
    win = stdscr; // Use stdscr if no window is provided
  }

  // Allocate memory for the form fields
  hash_form_field = (FIELD **)calloc((size_t)(hash_form_field_metadata_len + 2), sizeof(FIELD *));

  // Find the longest label length for the fields
  for (unsigned short i = 0; i < hash_form_field_metadata_len; ++i) {
    unsigned short label_length = strlen(hash_form_field_metadata[i].label);
    if (label_length > max_label_length)
      max_label_length = label_length;
  }

  // Get the longest max_length from the fields
  unsigned short max_field_length =
      calculate_longest_max_length(hash_form_field_metadata, hash_form_field_metadata_len, false);

  for (unsigned short i = 0; i < hash_form_field_metadata_len; ++i) {
    hash_form_field[i] = new_field(1,
                                   max_field_length + 1,
                                   i,
                                   BH_FORM_X_PADDING + BH_FORM_FIELD_BRACKET_PADDING +
                                       max_label_length + BH_FORM_FIELD_BRACKET_PADDING,
                                   0,
                                   0);

    // Convert the default value to string
    char *string_buffer = (char *)malloc(sizeof(char) * hash_form_field_metadata[i].max_length + 1);
    snprintf(string_buffer,
             hash_form_field_metadata[i].max_length + 1,
             "%hu",
             hash_form_field_metadata[i].default_value);

    // Make the field visible and editable
    field_opts_on(hash_form_field[i], O_STATIC);    // Keep field static size
    field_opts_off(hash_form_field[i], O_AUTOSKIP); // Don't auto skip to next field
    set_field_back(hash_form_field[i],
                   A_NORMAL); // Set normal background for all fields initially
    set_field_buffer(hash_form_field[i], 0, string_buffer); // Set the default value for the field
    set_field_just(hash_form_field[i], JUSTIFY_LEFT);       // Left justify the content

    free(string_buffer);

    // Set maximum field length
    set_max_field(hash_form_field[i], max_field_length);

    // Set the field type to numeric
    int max_value = calculate_form_max_value(hash_form_field_metadata[i].max_length);
    set_field_type(hash_form_field[i], TYPE_INTEGER, 0, (long)1, (long)max_value);
  }

  // Create the submit button field
  hash_form_field[hash_form_field_metadata_len] =
      create_button_field(hash_form_submit_button_text, // Button label
                          hash_form_field_metadata_len + BH_FORM_Y_PADDING,
                          BH_FORM_X_PADDING);

  // Add a NULL terminator to the field array
  hash_form_field[hash_form_field_metadata_len + 1] = NULL;

  // Create a sub-window for the form with extra space for the button
  hash_collision_form_sub_win = derwin(win, hash_form_field_metadata_len + 6, COLS - 4, 9, 1);
  keypad(hash_collision_form_sub_win, TRUE);

  // Create the form
  hash_collision_form = new_form(hash_form_field);
  set_form_win(hash_collision_form, win);
  set_form_sub(hash_collision_form, hash_collision_form_sub_win);
  post_form(hash_collision_form);

  set_current_field(hash_collision_form, hash_form_field[0]);
  update_field_highlighting(hash_collision_form,
                            hash_form_field_metadata_len + 1,
                            (unsigned short[]){hash_form_field_metadata_len},
                            1);
}

FORM *hash_collision_form_render(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    win = stdscr; // Use stdscr if no window is provided

  if (hash_collision_form == NULL)
    hash_collision_form_init(win); // Initialize the form if not already done

  // Set the label for the field
  for (unsigned short i = 0; i < hash_form_field_metadata_len; ++i) {
    mvwprintw(hash_collision_form_sub_win, i, BH_FORM_X_PADDING, hash_form_field_metadata[i].label);
    mvwprintw(hash_collision_form_sub_win, i, BH_FORM_X_PADDING + max_label_length, ": [");
    mvwprintw(hash_collision_form_sub_win,
              i,
              BH_FORM_X_PADDING + max_label_length + BH_FORM_FIELD_BRACKET_PADDING + 1 +
                  calculate_longest_max_length(
                      hash_form_field_metadata, hash_form_field_metadata_len, true) +
                  BH_FORM_FIELD_BRACKET_PADDING,
              "]");
  }

  form_driver(hash_collision_form, REQ_END_LINE);

  wrefresh(win);

  return hash_collision_form;
}

void hash_form_handle_input(WINDOW *win, int ch) {
  FIELD *current = current_field(hash_collision_form);
  int current_index = field_index(current);
  unsigned short longest_max_length_pad =
      calculate_longest_max_length(hash_form_field_metadata, hash_form_field_metadata_len, true);

  switch (ch) {
  case KEY_UP:
  case KEY_DOWN: {
    int result = form_driver(hash_collision_form, REQ_VALIDATION);
    if (ch == KEY_DOWN) {
      form_driver(hash_collision_form, REQ_NEXT_FIELD);
    } else {
      form_driver(hash_collision_form, REQ_PREV_FIELD);
    }
    form_driver(hash_collision_form, REQ_END_LINE);

    current = current_field(hash_collision_form);
    current_index = field_index(current);

    if (result == E_INVALID_FIELD) {
      display_field_error(
          hash_collision_form_sub_win,
          current,
          current_index,
          max_label_length,
          longest_max_length_pad,
          calculate_form_max_value(hash_form_field_metadata[current_index].max_length),
          false);
    } else {
      clear_field_error(
          hash_collision_form_sub_win, current_index, max_label_length, longest_max_length_pad);
    }

    update_field_highlighting(hash_collision_form,
                              hash_form_field_metadata_len + 1,
                              (unsigned short[]){hash_form_field_metadata_len},
                              1);

    if (current_index < hash_form_field_metadata_len) {
      pos_form_cursor(hash_collision_form);
    } else {
      set_field_buffer(hash_collision_form_field_get(hash_form_field_metadata_len),
                       0,
                       hash_form_submit_button_text);
      pos_form_cursor(hash_collision_form);
    }
  } break;

  case KEY_LEFT:
    if (current_index < hash_form_field_metadata_len) {
      form_driver(hash_collision_form, REQ_PREV_CHAR);
    }
    break;
  case KEY_RIGHT:
    if (current_index < hash_form_field_metadata_len) {
      form_driver(hash_collision_form, REQ_NEXT_CHAR);
    }
    break;

  case KEY_BACKSPACE:
  case 127:
    form_driver(hash_collision_form, REQ_DEL_PREV);
    break;
  case KEY_DC:
    if (current_index < hash_form_field_metadata_len) {
      form_driver(hash_collision_form, REQ_DEL_CHAR);
    }
    break;

  case '\n': {
    int result = form_driver(hash_collision_form, REQ_VALIDATION);

    if (result == E_INVALID_FIELD) {
      display_field_error(
          hash_collision_form_sub_win,
          current,
          current_index,
          max_label_length,
          longest_max_length_pad,
          calculate_form_max_value(hash_form_field_metadata[current_index].max_length),
          true);
    } else if (current_index == hash_form_field_metadata_len) {
      hash_form_validate_all_fields(win);
    }
  } break;

  default:
    if (current_index < hash_form_field_metadata_len && isdigit(ch))
      form_driver(hash_collision_form, ch);
    break;
  }
}