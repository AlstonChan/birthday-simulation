#include <ctype.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hash_collision.h"
#include "hash_config.h"

#include "../../utils/hash_function.h"
#include "../../utils/utils.h"
#include "../form.h"
#include "../layout.h"

static const char *hash_collision_page_title = "[ Hash Collision Demonstration ]";
static const char const *hash_form_submit_button_text = "[ Run Simulation ]";

static const struct FormInputField const hash_form_field_metadata[] = {{"Max Attempts", 10000, 6}};
static const unsigned short hash_form_field_metadata_len = ARRAY_SIZE(hash_form_field_metadata);

/**
 * @brief The longest field label length in the hash form.
 *
 */
static unsigned short max_label_length = 0;

static FIELD **hash_form_field = NULL;
static FORM *hash_collision_form = NULL;
static WINDOW *hash_collision_form_sub_win = NULL;

static FIELD *hash_collision_form_field_get(int index) { return hash_form_field[index]; }

/**************************************************************
                      HASH TABLE FUNCTIONS
**************************************************************/

/**
 * @brief Create a new hash table with the specified number of buckets.
 *
 * @param bucket_count The number of buckets in the hash table.
 * @return A pointer to the newly created hash table, or NULL on failure.
 */
static hash_table_t *hash_table_create(size_t bucket_count) {
  // First, allocate memory for the hash table structure
  hash_table_t *table = malloc(sizeof(hash_table_t));
  if (!table)
    return NULL;

  // Then, allocate memory for the hash_node_t pointers in the buckets array
  table->buckets = calloc(bucket_count, sizeof(hash_node_t *));
  if (!table->buckets) {
    free(table);
    return NULL;
  }

  table->bucket_count = bucket_count;
  return table;
}

/**
 * @brief Compute the hash for the given input based on the specified hash function ID.
 *
 * @param hash_id The ID of the hash function to use.
 * @param input The input data to hash, it should be a pointer to an array of bytes.
 * @param input_len The length of the input data in bytes.
 * @param output A pointer to a string where the computed hash will be stored in hexadecimal format.
 * @return true The hash was computed successfully.
 * @return false Memory allocation failed or an unsupported hash function ID was provided.
 */
static bool compute_hash(enum hash_function_ids hash_id, const uint8_t *input, size_t input_len,
                         char **output) {
  size_t hash_hex_len = get_hash_hex_length(hash_id);
  *output = malloc(hash_hex_len);

  switch (hash_id) {
  case HASH_CONFIG_8BIT: {
    uint8_t result = hash_8bit(input, input_len);
    sprintf(*output, "%02X", result);
  } break;
  case HASH_CONFIG_12BIT: {
    uint16_t result = hash_12bit(input, input_len);
    sprintf(*output, "%03X", result);
  } break;
  case HASH_CONFIG_16BIT: {
    uint16_t result = hash_16bit(input, input_len);
    sprintf(*output, "%04X", result);
  } break;
  case HASH_CONFIG_RIPEMD160:
  case HASH_CONFIG_SHA1:
  case HASH_CONFIG_SHA3_256:
  case HASH_CONFIG_SHA256:
  case HASH_CONFIG_SHA512:
  case HASH_CONFIG_SHA384: {
    int openssl_id;
    switch (hash_id) {
    case HASH_CONFIG_RIPEMD160:
      openssl_id = BH_OPENSSL_HASH_RIPEMD160;
      break;
    case HASH_CONFIG_SHA1:
      openssl_id = BH_OPENSSL_HASH_SHA1;
      break;
    case HASH_CONFIG_SHA3_256:
      openssl_id = BH_OPENSSL_HASH_SHA3_256;
      break;
    case HASH_CONFIG_SHA256:
      openssl_id = BH_OPENSSL_HASH_SHA256;
      break;
    case HASH_CONFIG_SHA512:
      openssl_id = BH_OPENSSL_HASH_SHA512;
      break;
    case HASH_CONFIG_SHA384:
      openssl_id = BH_OPENSSL_HASH_SHA384;
      break;
    default:
      free(*output);
      return false;
    }

    unsigned char *digest = openssl_hash(input, input_len, openssl_id);
    if (!digest) {
      free(*output);
      return false;
    }

    size_t bin_len = hash_hex_len - 1;          // Exclude null terminator
    *output = bytes_to_hex(digest, bin_len, 1); // Convert to hex string
    (*output)[hash_hex_len - 1] = '\0';         // Null-terminate the string
    if (!*output) {
      free(digest);
      return false; // Memory allocation failed in bytes_to_hex
    }

    free(digest); // Free the binary digest after conversion
  } break;
  }

  return true;
}

/**
 * @brief Uses djb2 algorithm to compute a simple hash for the given string.
 *
 * @param str The string to hash.
 * @param bucket_count The number of buckets in the hash table.
 * @return size_t
 */
static size_t simple_hash(const char *str, size_t bucket_count) {
  size_t hash = 5381;
  int c;
  while ((c = *str++)) {             // Loops auto terminates at null character
    hash = ((hash << 5) + hash) + c; // Multiply by 33 and add the current character
  }
  return hash % bucket_count;
}

static hash_node_t *hash_table_find(hash_table_t *table, const char *hash_hex) {
  size_t bucket = simple_hash(hash_hex, table->bucket_count);
  hash_node_t *entry = table->buckets[bucket];

  while (entry) {
    if (strcmp(entry->hash_hex, hash_hex) == 0) {
      return entry; // Found the entry with the matching hash
    }
    entry = entry->next; // Move to the next node in the linked list to continue searching
  }
  return NULL;
}

static bool hash_table_insert(hash_table_t *table, const char *input, const char *hash_hex) {
  size_t bucket = simple_hash(hash_hex, table->bucket_count);

  hash_node_t *entry = malloc(sizeof(hash_node_t));
  if (!entry)
    return false;

  entry->input = strdup(input);
  entry->hash_hex = strdup(hash_hex);
  if (!entry->input || !entry->hash_hex) {
    free(entry->input);
    free(entry->hash_hex);
    free(entry);
    return false;
  }

  entry->next = table->buckets[bucket];
  table->buckets[bucket] = entry;
  return true;
}

static void hash_table_destroy(hash_table_t *table) {
  if (!table)
    return;

  for (size_t i = 0; i < table->bucket_count; i++) {
    hash_node_t *entry = table->buckets[i];
    while (entry) {
      hash_node_t *next = entry->next;
      free(entry->input);
      free(entry->hash_hex);
      free(entry);
      entry = next;
    }
  }

  free(table->buckets);
  free(table);
}

static hash_collision_simulation_result_t *
hash_collision_simulation_run(enum hash_function_ids hash_id, int max_attempts) {
  const size_t hash_space_size = 1 << (int)get_hash_config_item(hash_id).bits;

  if (max_attempts <= 0) {
    max_attempts = 10000; // Default to 10,000 attempts for negative or zero attempts
  }

  // Initialize result structure
  hash_collision_simulation_result_t *result = malloc(sizeof(hash_collision_simulation_result_t));
  if (!result) {
    fprintf(stderr, "Memory allocation failed for hash collision simulation result.\n");
    exit(EXIT_FAILURE);
  }

  // Initialize result fields
  result->id = hash_id;
  result->attempts_made = 0; // Cast away const for initialization
  result->collision_found = false;
  result->collision_input_1 = NULL;
  result->collision_input_2 = NULL;
  result->collision_hash_hex = NULL;

  size_t hash_hex_len = get_hash_hex_length(hash_id);

  // The desired table size is 1.3 times the maximum attempts so that
  // the load factor (n / table_size) should ideally stay under 0.75.
  size_t desired_table_size = (size_t)(max_attempts * 1.3);
  size_t table_size = next_prime(desired_table_size);
  hash_table_t *table = hash_table_create(table_size);

  if (!table) {
    fprintf(stderr, "Memory allocation failed for hash table.\n");
    free(result);
    exit(EXIT_FAILURE);
  }

  // Birthday Attack simulation core logic
  for (unsigned int attempt = 0; attempt < max_attempts; ++attempt) {
    result->attempts_made = attempt;

    // Step 1: Generate a random input
    uint8_t current_input[32];
    size_t input_len = generate_random_input(current_input, 4, 31);

    // Step 2: Compute the hash of this input
    char *hash_hex = malloc(hash_hex_len);
    compute_hash(hash_id, current_input, input_len, &hash_hex);

    // Step 3: Birthday Attack Core - Check if this hash already exists
    hash_node_t *existing = hash_table_find(table, hash_hex);

    if (existing) {
      // Collision found. BIRTHDAY ATTACK SUCCESS: Same hash with different inputs!
      result->collision_found = true;
      result->collision_input_1 = strdup(existing->input);
      result->collision_input_2 = strdup(bytes_to_hex(current_input, input_len, 1));
      result->collision_hash_hex = strdup(hash_hex);

      // Free the hash hex before exiting the loop
      free(hash_hex);
      break;
    } else {
      // No collision, insert the new hash into the table
      hash_table_insert(table, bytes_to_hex(current_input, input_len, 1), hash_hex);
    }

    // Free the hash hex after each attempt
    free(hash_hex);
  }

  // Cleanup: Free the hash table and its entries
  hash_table_destroy(table);
  return result;
}

/**************************************************************
                    FORM HANDLING FUNCTIONS
**************************************************************/

static bool hash_form_validate_all_fields(WINDOW *win, enum hash_function_ids hash_id) {
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
    hash_collision_simulation_result_t *results = hash_collision_simulation_run(hash_id, attempts);

    uint8_t starting_y = hash_form_field_metadata_len + 1 + 2;

    // Clear the sub-window from starting_y to starting_y + 5
    for (unsigned short row = starting_y; row < starting_y + 6; ++row) {
      for (int col = BH_FORM_X_PADDING; col <= COLS - BH_FORM_X_PADDING; col++) {
        mvwaddch(hash_collision_form_sub_win, row, col, ' ');
      }
    }

    if (results->collision_found) {
      // Display the results of the collision simulation
      wattron(hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
      mvwprintw(hash_collision_form_sub_win,
                starting_y,
                BH_FORM_X_PADDING,
                "Collision Found at attempt %d!",
                results->attempts_made);
      wattroff(hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
      mvwprintw(hash_collision_form_sub_win,
                starting_y + 1,
                BH_FORM_X_PADDING,
                "Input 1: %s",
                results->collision_input_1);
      mvwprintw(hash_collision_form_sub_win,
                starting_y + 2,
                BH_FORM_X_PADDING,
                "Input 2: %s",
                results->collision_input_2);
      mvwprintw(hash_collision_form_sub_win,
                starting_y + 3,
                BH_FORM_X_PADDING,
                "Hash   : %s",
                results->collision_hash_hex);
    } else {
      wattron(hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_ERROR_COLOR_PAIR));
      mvwprintw(hash_collision_form_sub_win,
                starting_y,
                BH_FORM_X_PADDING,
                "No Collision Found after %d attempts.",
                attempts);
      wattroff(hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_ERROR_COLOR_PAIR));
    }
  }

  return all_valid;
}

static void hash_collision_form_init(WINDOW *win) {
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
                          hash_form_field_metadata_len + 1,
                          BH_FORM_X_PADDING);

  // Add a NULL terminator to the field array
  hash_form_field[hash_form_field_metadata_len + 1] = NULL;

  // Create a sub-window for the form with extra space for the button
  hash_collision_form_sub_win = derwin(
      win, hash_form_field_metadata_len + 12, COLS - BH_FORM_X_PADDING - BH_FORM_X_PADDING, 9, 1);
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

static FORM *hash_collision_form_render(WINDOW *win, int max_y, int max_x) {
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

static void hash_form_handle_input(WINDOW *win, enum hash_function_ids hash_id, int ch) {
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
      hash_form_validate_all_fields(win, hash_id);
    }
  } break;

  default:
    if (current_index < hash_form_field_metadata_len && isdigit(ch))
      form_driver(hash_collision_form, ch);
    break;
  }
}

/**************************************************************
                      EXTERNAL FUNCTIONS
**************************************************************/

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
  mvwprintw(win, 3, BH_FORM_X_PADDING, "Hash output bits    : %u bits", current_hash_function.bits);
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
    hash_form_handle_input(win, hash_id, char_input); // Handle user input for the form
  }

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);

  curs_set(0);         // Hide the cursor
  nodelay(win, FALSE); // Make getch() blocking
}