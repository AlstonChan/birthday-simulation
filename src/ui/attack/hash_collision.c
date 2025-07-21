/**
 * \file            hash_collision.c
 * \brief           The main component for attack.c page to render the UI and execute
 *                  the logic of creating hashs, comparing hashs, to simulate a 
 *                  birthday attack
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "hash_collision.h"

static const char* s_hash_collision_page_title = "[ Hash Collision Demonstration ]";
static const char const* s_hash_form_submit_button_text = "[ Run Simulation ]";

static const struct FormInputField const s_hash_form_field_metadata[] = {
    {"Max Attempts", 10000, 6}};
static const unsigned short s_hash_form_field_metadata_len = ARRAY_SIZE(s_hash_form_field_metadata);
static unsigned short s_max_label_length = 0;

/**
 * \brief          Holds all the fields for the hash collision form. ALl fields
 *                 holds input data for the hash collision simulation with special case:
 *                 - The second last field is a button field that submits the form, that
 *                 it cannot be used for input.
 *                 - The last field is a NULL terminator field that is used to
 *                 terminate the array of fields.
 *
 */
static FIELD** s_hash_form_field = NULL;
static FORM* s_hash_collision_form = NULL;
static WINDOW* s_hash_collision_form_sub_win = NULL;

static FIELD*
hash_collision_form_field_get(int index) {
    return s_hash_form_field[index];
}

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/

/**************************************************************
                      HASH TABLE FUNCTIONS
**************************************************************/

/**
 * \brief          Create a new hash table with the specified number of buckets.
 *                 You should free the returned hash table using `hash_table_destroy`
 *                 when done.
 *
 * \param[in]      bucket_count The number of buckets in the hash table.
 * \return         A pointer to the newly created hash table, or NULL on memory
 *                 allocation failure
 */
static hash_table_t*
hash_table_create(size_t bucket_count) {
    // First, allocate memory for the hash table structure
    hash_table_t* table = malloc(sizeof(hash_table_t));
    if (!table) {
        return NULL;
    }

    // Then, allocate memory for the hash_node_t pointers in the buckets array
    table->buckets = calloc(bucket_count, sizeof(hash_node_t*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->bucket_count = bucket_count;
    return table;
}

/**
 * \brief          Compute the hash for the given input based on the specified hash function ID.
 *
 * \param[in]      hash_id The ID of the hash function to use.
 * \param[in]      input The input data to hash, it should be a pointer to an array of bytes.
 * \param[in]      input_len The length of the input data in bytes.
 * \param[out]     output A pointer to a string where the computed hash will be stored in hexadecimal format.
 * \return         true The hash was computed successfully.
 * \return         false Memory allocation failed or an unsupported hash function ID was provided.
 */
static bool
compute_hash(enum hash_function_ids hash_id, const uint8_t* input, size_t input_len,
             char** output) {
    size_t hash_hex_len = get_hash_hex_length(hash_id);
    *output = malloc(hash_hex_len);
    if (!output) {
        return false;
    }

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

            // First, assign the OpenSSL ID based on the hash_id
            switch (hash_id) {
                case HASH_CONFIG_RIPEMD160: openssl_id = BH_OPENSSL_HASH_RIPEMD160; break;
                case HASH_CONFIG_SHA1: openssl_id = BH_OPENSSL_HASH_SHA1; break;
                case HASH_CONFIG_SHA3_256: openssl_id = BH_OPENSSL_HASH_SHA3_256; break;
                case HASH_CONFIG_SHA256: openssl_id = BH_OPENSSL_HASH_SHA256; break;
                case HASH_CONFIG_SHA512: openssl_id = BH_OPENSSL_HASH_SHA512; break;
                case HASH_CONFIG_SHA384: openssl_id = BH_OPENSSL_HASH_SHA384; break;
                default: free(*output); return false;
            }

            // Then, compute the hash using OpenSSL given the OpenSSL ID
            unsigned char* digest = openssl_hash(input, input_len, openssl_id);
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
 * \brief          Uses djb2 algorithm to compute a simple hash for the given string.
 *
 * \param[in]      str The string to hash.
 * \param[in]      bucket_count The number of buckets in the hash table.
 * \return         size_t
 */
static size_t
simple_hash(const char* str, size_t bucket_count) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {               // Loops auto terminates at null character
        hash = ((hash << 5) + hash) + c; // Multiply by 33 and add the current character
    }
    return hash % bucket_count;
}

/**
 * \brief          Finds an entry in the hash table by its hash value.
 *                 This function searches for a hash value in the hash table and returns the corresponding
 *                 hash_node_t if found, or NULL if not found.
 *
 * \param[in]      table The hash table to search in.
 * \param[in]      hash_hex The hexadecimal string representation of the hash to find.
 * \return         A pointer to the hash_node_t if found, or NULL if not found.
 */
static hash_node_t*
hash_table_find(hash_table_t* table, const char* hash_hex) {
    size_t bucket = simple_hash(hash_hex, table->bucket_count);
    hash_node_t* entry = table->buckets[bucket];

    while (entry) {
        if (strcmp(entry->hash_hex, hash_hex) == 0) {
            return entry; // Found the entry with the matching hash
        }
        entry = entry->next; // Move to the next node in the linked list to continue searching
    }
    return NULL;
}

/**
 * \brief          Inserts a new entry into the hash table.
 *                 This function creates a new hash_node_t, initializes it with the input and hash_hex,
 *                 and inserts it into the appropriate bucket in the hash table.
 *
 * \param[in]      table The hash table to insert into.
 * \param[in]      input The input string that generated the hash.
 * \param[in]      hash_hex The hexadecimal string representation of the hash.
 * \return         true if the insertion was successful, false otherwise (e.g., memory allocation failure).
 */
static bool
hash_table_insert(hash_table_t* table, const char* input, const char* hash_hex) {
    size_t bucket = simple_hash(hash_hex, table->bucket_count);

    hash_node_t* entry = malloc(sizeof(hash_node_t));
    if (!entry) {
        return false;
    }

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

/**
 * \brief          Destroys the hash table and frees all its resources.
 *                 This function iterates through each bucket in the hash table, freeing
 *                 each linked list entry and its associated resources. Finally,
 *                 it frees the buckets array and the hash table itself.
 *
 * \param[in]      table The hash table to destroy.
 *
 */
static void
hash_table_destroy(hash_table_t* table) {
    // No table to destroy, return early
    if (!table) {
        return;
    }

    // Loop over each bucket in the hash table
    for (size_t i = 0; i < table->bucket_count; i++) {
        // Gets the head of the linked list for this bucket
        hash_node_t* entry = table->buckets[i];

        // If the bucket is empty, continue to the next bucket
        while (entry) {
            // Get the next entry before freeing the current one
            hash_node_t* next = entry->next;

            // Free the current entry's resources
            free(entry->input);
            free(entry->hash_hex);
            free(entry);

            // Move to the next entry in the linked list
            entry = next;
        }
    }

    // Finally, free the hash table's buckets array and the table itself
    free(table->buckets);
    free(table);
}

/**
 * \brief          Simulates a hash collision using the Birthday Attack algorithm.
 *                 It will first create a hash table with a size based on the maximum number of attempts.
 *                 Then, it will generate random inputs, compute their hashes, and check for collisions.
 *                 If a collision is found, it will store the inputs and the hash in the result structure.
 *                 If no collision is found after the maximum number of attempts, it will return a result
 *                 indicating no collision.
 *
 * \param[in]      hash_id The ID of the hash function to use for the simulation.
 * \param[in]      max_attempts The maximum number of attempts to find a collision before exiting.
 * \return         hash_collision_simulation_result_t*
 */
static hash_collision_simulation_result_t*
hash_collision_simulation_run(enum hash_function_ids hash_id, unsigned int max_attempts) {
    if (max_attempts <= 0) {
        max_attempts = 10000; // Default to 10,000 attempts for negative or zero attempts
    }

    // Initialize result structure
    hash_collision_simulation_result_t* result = malloc(sizeof(hash_collision_simulation_result_t));
    if (!result) {
        render_full_page_error_exit(
            stdscr, 0, 0, "Memory allocation failed for hash collision simulation result.");
    }

    // Initialize result fields
    result->id = hash_id;
    result->attempts_made = 0;
    result->collision_found = false;
    result->collision_input_1 = NULL;
    result->collision_input_2 = NULL;
    result->collision_hash_hex = NULL;

    uint16_t hash_hex_len = get_hash_hex_length(hash_id);

    // The desired table size is 1.3 times the maximum attempts so that
    // the load factor (n / table_size) should ideally stay under 0.75.
    unsigned int desired_table_size = (max_attempts * 1.3);
    unsigned int table_size = next_prime(desired_table_size);
    hash_table_t* table = hash_table_create(table_size);

    if (!table) {
        free(result);
        render_full_page_error_exit(stdscr, 0, 0, "Memory allocation failed for hash table.");
    }

    // Birthday Attack simulation core logic
    for (unsigned int attempt = 0; attempt < max_attempts; ++attempt) {
        result->attempts_made = attempt;

        // Step 1: Generate a random input
        uint8_t current_input[32];
        size_t input_len = generate_random_input(current_input, 4, 31);

        // Step 2: Compute the hash of this input
        char* hash_hex = malloc(hash_hex_len);
        bool compute_success = compute_hash(hash_id, current_input, input_len, &hash_hex);
        if (!compute_success) {
            free(hash_hex);
            free(result);
            render_full_page_error_exit(stdscr, 0, 0, "compute_hash was unsuccessful");
        }

        // Step 3: Birthday Attack Core - Check if this hash already exists
        hash_node_t* existing = hash_table_find(table, hash_hex);

        if (existing) {
            // Collision found. BIRTHDAY ATTACK SUCCESS: Same hash with different inputs!
            result->collision_found = true;
            result->collision_input_1 = strdup(existing->input);
            result->collision_input_2 = strdup(bytes_to_hex(current_input, input_len, true));
            result->collision_hash_hex = strdup(hash_hex);

            // Free the hash hex before exiting the loop
            free(hash_hex);
            break;
        } else {
            // No collision, insert the new hash into the table
            bool insert_success =
                hash_table_insert(table, bytes_to_hex(current_input, input_len, true), hash_hex);
            if (!insert_success) {
                free(hash_hex);
                free(result);
                render_full_page_error_exit(stdscr, 0, 0,
                                            "Hash insertion into the hash table failed.");
            }
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

/**
 * \brief          Create a sub window from the parent window for the form
 *
 * \param[in]      win The window that will contain the created subwin for the form. This
 *                 should ideally be the content win
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 */
static void
hash_form_create_sub_win(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to hash_form_create_sub_win is null");
    }

    if (s_hash_collision_form_sub_win) {
        delwin(s_hash_collision_form_sub_win);
        s_hash_collision_form_sub_win = NULL;
    }

    const int sub_win_rows_count = s_hash_form_field_metadata_len + 12;
    const int sub_win_cols_count = max_x - BH_FORM_X_PADDING - BH_FORM_X_PADDING;

    // Create a sub-window for the form with extra space for the button
    s_hash_collision_form_sub_win = derwin(win, sub_win_rows_count, sub_win_cols_count, 9, 1);
    keypad(s_hash_collision_form_sub_win, TRUE);

    set_form_win(s_hash_collision_form, win);
    set_form_sub(s_hash_collision_form, s_hash_collision_form_sub_win);

    unpost_form(s_hash_collision_form); // Safeguard against state issues
    post_form(s_hash_collision_form);
}

/**
 * \brief          Take the value from the form field as arguments for simulating the
 *                 birthday attack. The result will be stored back to the arguments
 *                 provided to the function.
 *
 * \param[in]      hash_id The hash id of the hash function to use to simulate
 */
static hash_collision_simulation_result_t*
run_hash_collision_from_input(enum hash_function_ids hash_id) {
    unsigned int attempts = atoi(field_buffer(hash_collision_form_field_get(0), 0));
    return hash_collision_simulation_run(hash_id, attempts);
}

/**
 * \brief          Render the birthday attack result given the value
 *
 * \param[in]      results The final results to render, either it is a successful hash collision
 *                 or no collision found
 */
static void
render_attack_result(hash_collision_simulation_result_t results) {
    uint8_t starting_y = s_hash_form_field_metadata_len + 1 + 2;

    // Clear the sub-window from starting_y to starting_y + 5
    for (unsigned short row = starting_y; row < starting_y + 6; ++row) {
        for (int col = BH_FORM_X_PADDING; col <= COLS - BH_FORM_X_PADDING; col++) {
            mvwaddch(s_hash_collision_form_sub_win, row, col, ' ');
        }
    }

    if (results.collision_found) {
        // Display the results of the collision simulation
        wattron(s_hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
        mvwprintw(s_hash_collision_form_sub_win, starting_y, BH_FORM_X_PADDING,
                  "Collision Found at attempt %d!", results.attempts_made);
        wattroff(s_hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
        mvwprintw(s_hash_collision_form_sub_win, starting_y + 1, BH_FORM_X_PADDING, "Input 1: %s",
                  results.collision_input_1);
        mvwprintw(s_hash_collision_form_sub_win, starting_y + 2, BH_FORM_X_PADDING, "Input 2: %s",
                  results.collision_input_2);
        mvwprintw(s_hash_collision_form_sub_win, starting_y + 3, BH_FORM_X_PADDING, "Hash   : %s",
                  results.collision_hash_hex);
    } else {
        wattron(s_hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_ERROR_COLOR_PAIR));
        mvwprintw(s_hash_collision_form_sub_win, starting_y, BH_FORM_X_PADDING,
                  "No Collision Found after %d attempts.", results.attempts_made);
        wattroff(s_hash_collision_form_sub_win, A_BOLD | COLOR_PAIR(BH_ERROR_COLOR_PAIR));
    }

    wrefresh(s_hash_collision_form_sub_win);
}

/**
 * \brief          Loop over all field and validate the field. Error message will
 *                 be displayed at the side of the field if any
 *
 * \return         true No error found, all field is valid
 * \return         false One or more input is invalid
 */
static bool
hash_form_validate_all_fields() {
    bool all_valid = true;
    unsigned short longest_max_length_pad = calculate_longest_max_length(
        s_hash_form_field_metadata, s_hash_form_field_metadata_len, true);

    for (unsigned short i = 0; i < s_hash_form_field_metadata_len; ++i) {
        FIELD* field = hash_collision_form_field_get(i);
        int result = form_driver(s_hash_collision_form, REQ_VALIDATION);

        if (result == E_INVALID_FIELD) {
            display_field_error(
                s_hash_collision_form_sub_win, field, i, s_max_label_length, longest_max_length_pad,
                calculate_form_max_value(s_hash_form_field_metadata[i].max_length), true);
            all_valid = false;
        } else {
            clear_field_error(s_hash_collision_form_sub_win, i, s_max_label_length,
                              longest_max_length_pad);
        }
    }

    if (all_valid) {
        set_current_field(s_hash_collision_form,
                          hash_collision_form_field_get(s_hash_form_field_metadata_len));
    } else {
        set_current_field(s_hash_collision_form, hash_collision_form_field_get(0));
    }

    return all_valid;
}

/**
 * \brief          Initializes the hash collision form with the given window.
 *
 * \param[in]      win The window to display the form in. This should ideally be
 *                 the content window.
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 */
static void
hash_collision_form_init(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to hash_collision_form_init is null");
    }

    if (s_hash_collision_form != NULL) {
        render_full_page_error_exit(
            win, 0, 0,
            "The hash collision form has already been initialized and another "
            "attempt to initialize is not permitted");
    }

    // Allocate memory for the form fields
    s_hash_form_field =
        (FIELD**)calloc((size_t)(s_hash_form_field_metadata_len + 2), sizeof(FIELD*));

    // Find the longest label length for the fields
    for (unsigned short i = 0; i < s_hash_form_field_metadata_len; ++i) {
        unsigned short label_length = strlen(s_hash_form_field_metadata[i].label);
        if (label_length > s_max_label_length) {
            s_max_label_length = label_length;
        }
    }

    // Get the longest max_length from the fields
    unsigned short max_field_length = calculate_longest_max_length(
        s_hash_form_field_metadata, s_hash_form_field_metadata_len, false);

    for (unsigned short i = 0; i < s_hash_form_field_metadata_len; ++i) {
        s_hash_form_field[i] = new_field(1, max_field_length + 1, i,
                                         BH_FORM_X_PADDING + BH_FORM_FIELD_BRACKET_PADDING
                                             + s_max_label_length + BH_FORM_FIELD_BRACKET_PADDING,
                                         0, 0);

        // Convert the default value to string
        char* string_buffer =
            (char*)malloc(sizeof(char) * s_hash_form_field_metadata[i].max_length + 1);
        if (!string_buffer) {
            render_full_page_error_exit(
                stdscr, 0, 0, "Memory allocation failed for hash collision form default value");
        }

        snprintf(string_buffer, s_hash_form_field_metadata[i].max_length + 1, "%hu",
                 s_hash_form_field_metadata[i].default_value);

        // Make the field visible and editable
        field_opts_on(s_hash_form_field[i], O_STATIC);    // Keep field static size
        field_opts_off(s_hash_form_field[i], O_AUTOSKIP); // Don't auto skip to next field
        set_field_back(s_hash_form_field[i],
                       A_NORMAL); // Set normal background for all fields initially
        set_field_buffer(s_hash_form_field[i], 0,
                         string_buffer);                    // Set the default value for the field
        set_field_just(s_hash_form_field[i], JUSTIFY_LEFT); // Left justify the content

        free(string_buffer);

        // Set maximum field length
        set_max_field(s_hash_form_field[i], max_field_length);

        // Set the field type to numeric
        int max_value = calculate_form_max_value(s_hash_form_field_metadata[i].max_length);
        set_field_type(s_hash_form_field[i], TYPE_INTEGER, 0, (long)1, (long)max_value);
    }

    // Create the submit button field
    s_hash_form_field[s_hash_form_field_metadata_len] =
        create_button_field(s_hash_form_submit_button_text, // Button label
                            s_hash_form_field_metadata_len + 1, BH_FORM_X_PADDING);

    // Add a NULL terminator to the field array
    s_hash_form_field[s_hash_form_field_metadata_len + 1] = NULL;

    // Create the form
    s_hash_collision_form = new_form(s_hash_form_field);

    set_current_field(s_hash_collision_form, s_hash_form_field[0]);
    update_field_highlighting(s_hash_collision_form, s_hash_form_field_metadata_len + 1,
                              (unsigned short[]){s_hash_form_field_metadata_len}, 1);

    hash_form_create_sub_win(win, max_y, max_x);
}

/**
 * \brief          Renders the hash collision form in the given window.
 *
 * \param[in]      win The window to render the form in. This should ideally be
 *                 the content window.
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 * \return         The form instance that is being rendered
 */
static FORM*
hash_collision_form_render(WINDOW* win, int max_y, int max_x) {
    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to hash_collision_form_render is null");
    }

    if (s_hash_collision_form == NULL) {
        hash_collision_form_init(win, max_y, max_x); // Initialize the form if not already done
    }

    // Set the label for the field
    for (unsigned short i = 0; i < s_hash_form_field_metadata_len; ++i) {
        mvwprintw(s_hash_collision_form_sub_win, i, BH_FORM_X_PADDING,
                  s_hash_form_field_metadata[i].label);
        mvwprintw(s_hash_collision_form_sub_win, i, BH_FORM_X_PADDING + s_max_label_length, ": [");
        mvwprintw(s_hash_collision_form_sub_win, i,
                  BH_FORM_X_PADDING + s_max_label_length + BH_FORM_FIELD_BRACKET_PADDING + 1
                      + calculate_longest_max_length(s_hash_form_field_metadata,
                                                     s_hash_form_field_metadata_len, true)
                      + BH_FORM_FIELD_BRACKET_PADDING,
                  "]");
    }

    form_driver(s_hash_collision_form, REQ_END_LINE);

    wrefresh(win);

    return s_hash_collision_form;
}

/**
 * \brief          Restore the form to the window, that has previously
 *                 been cleared
 *
 * \param[in]      win The window that should restore the form to.
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 * \param[in]      result The results of the previous render to be restore in this frame
 */
static void
hash_collision_form_restore(WINDOW* win, int max_y, int max_x,
                            hash_collision_simulation_result_t result) {
    if (!s_hash_collision_form) {
        return;
    }

    if (win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to hash_collision_form_restore is null");
    }

    // recreate the sub win
    hash_form_create_sub_win(win, max_y, max_x);

    // Force re-render of field labels
    hash_collision_form_render(win, max_y, max_x);

    // Manually restore field buffers
    for (int i = 0; s_hash_form_field[i] != NULL; ++i) {
        const char* buf = field_buffer(s_hash_form_field[i], 0);
        set_field_buffer(s_hash_form_field[i], 0, buf); // Force internal repaint
    }

    // Force redraw current field again
    set_current_field(s_hash_collision_form, s_hash_form_field[0]);
    form_driver(s_hash_collision_form, REQ_FIRST_FIELD);

    if (result.attempts_made != -1) {
        render_attack_result(result);
    }

    wrefresh(s_hash_collision_form_sub_win);
}

/**
 * \brief          Destroys the hash collision form and frees the memory allocated for it.
 *
 */
static void
hash_collision_form_destroy() {
    if (s_hash_collision_form == NULL) {
        return; // If the form is already destroyed, do nothing
    }

    unpost_form(s_hash_collision_form);
    free_form(s_hash_collision_form);

    // Free the fields
    for (unsigned short i = 0; i < s_hash_form_field_metadata_len + 2; ++i) {
        free_field(s_hash_form_field[i]);
    }
    free_field(s_hash_form_field[s_hash_form_field_metadata_len]); // Free the submit button field
    free(s_hash_form_field[s_hash_form_field_metadata_len + 1]);   // Free the NULL terminator

    s_hash_form_field = NULL;
    s_hash_collision_form = NULL;
    s_hash_collision_form_sub_win = NULL;
}

/**
 * \brief          Handles input for the hash collision form.
 *
 * \param[in]      hash_id The current hash id of the hash function
 * \param[in]      ch The current int character input from the key pressed
 * \param[out]     collision_result The results of the birthday attack reference
 */
static void
hash_form_handle_input(enum hash_function_ids hash_id, int ch,
                       hash_collision_simulation_result_t** collision_result) {
    FIELD* current = current_field(s_hash_collision_form);
    int current_index = field_index(current);
    unsigned short longest_max_length_pad = calculate_longest_max_length(
        s_hash_form_field_metadata, s_hash_form_field_metadata_len, true);

    switch (ch) {
        case KEY_UP:
        case KEY_DOWN: {
            int result = form_driver(s_hash_collision_form, REQ_VALIDATION);
            if (ch == KEY_DOWN) {
                form_driver(s_hash_collision_form, REQ_NEXT_FIELD);
            } else {
                form_driver(s_hash_collision_form, REQ_PREV_FIELD);
            }
            form_driver(s_hash_collision_form, REQ_END_LINE);

            current = current_field(s_hash_collision_form);
            current_index = field_index(current);

            if (result == E_INVALID_FIELD) {
                display_field_error(
                    s_hash_collision_form_sub_win, current, current_index, s_max_label_length,
                    longest_max_length_pad,
                    calculate_form_max_value(s_hash_form_field_metadata[current_index].max_length),
                    false);
            } else {
                clear_field_error(s_hash_collision_form_sub_win, current_index, s_max_label_length,
                                  longest_max_length_pad);
            }

            update_field_highlighting(s_hash_collision_form, s_hash_form_field_metadata_len + 1,
                                      (unsigned short[]){s_hash_form_field_metadata_len}, 1);

            if (current_index < s_hash_form_field_metadata_len) {
                pos_form_cursor(s_hash_collision_form);
            } else {
                set_field_buffer(hash_collision_form_field_get(s_hash_form_field_metadata_len), 0,
                                 s_hash_form_submit_button_text);
                pos_form_cursor(s_hash_collision_form);
            }
        } break;

        case KEY_LEFT:
            if (current_index < s_hash_form_field_metadata_len) {
                form_driver(s_hash_collision_form, REQ_PREV_CHAR);
            }
            break;
        case KEY_RIGHT:
            if (current_index < s_hash_form_field_metadata_len) {
                form_driver(s_hash_collision_form, REQ_NEXT_CHAR);
            }
            break;

        case KEY_BACKSPACE:
        case 127: form_driver(s_hash_collision_form, REQ_DEL_PREV); break;
        case KEY_DC:
            if (current_index < s_hash_form_field_metadata_len) {
                form_driver(s_hash_collision_form, REQ_DEL_CHAR);
            }
            break;

        case '\n': {
            int result = form_driver(s_hash_collision_form, REQ_VALIDATION);

            if (result == E_INVALID_FIELD) {
                display_field_error(
                    s_hash_collision_form_sub_win, current, current_index, s_max_label_length,
                    longest_max_length_pad,
                    calculate_form_max_value(s_hash_form_field_metadata[current_index].max_length),
                    true);
            } else if (current_index == s_hash_form_field_metadata_len) {
                bool all_field_valid = hash_form_validate_all_fields();
                if (all_field_valid) {
                    // We must free the collision_result every time before we reassign a
                    // new result to it. This is because all the result struct assign to
                    // it is created with malloc, which is heap memory struct, and must
                    // manually freeing it to avoid memory leak.
                    free(*collision_result);
                    *collision_result = run_hash_collision_from_input(hash_id);
                    render_attack_result(**collision_result);
                }
            }
        } break;

        default:
            if (current_index < s_hash_form_field_metadata_len && isdigit(ch)) {
                form_driver(s_hash_collision_form, ch);
            }
            break;
    }
}

/**
 * \brief          Render the current's page hash function details
 *
 * \param[in]      content_win The window to actually prints all the details in
 * \param[in]      current_hash_function The current hash function configuration
 *                 as the main details to show
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 */
static void
render_page_details(WINDOW* content_win, hash_config_t current_hash_function, int max_x) {
    if (content_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_page_details is null");
    }

    // Display the hash function details
    mvwprintw(content_win, 2, BH_FORM_X_PADDING, "Hash Function       : %s",
              current_hash_function.label);
    mvwprintw(content_win, 3, BH_FORM_X_PADDING, "Hash output bits    : %u bits",
              current_hash_function.bits);
    mvwprintw(content_win, 4, BH_FORM_X_PADDING, "Estimated Collisions: %s",
              current_hash_function.estimated_collisions);
    mvwprintw(content_win, 5, BH_FORM_X_PADDING, "Space Size          : %s",
              current_hash_function.space_size);

    // Segment the details and form input fields with a line
    char* separator_line =
        (char*)malloc((max_x - 3) * sizeof(char)); // Allocate space for the separator line

    for (int i = 0; i < max_x - 4; i++) {
        separator_line[i] = '-';
    }
    separator_line[max_x - 4] = '\0';                             // Null-terminate the string
    mvwprintw(content_win, 7, BH_FORM_X_PADDING, separator_line); // Draw a line below the details

    free(separator_line);
}

/**************************************************************
                      EXTERNAL FUNCTIONS
**************************************************************/

/**
 * \brief          Render the hash collision page in the given window that allows
 *                 simulation of hash function to demonstrate collisions/no collisions.
 *
 * \param[in]      content_win The window to render the attack page on
 * \param[in]      header_win The window to render the header content, normally for
 *                 the args of header_render
 * \param[in]      footer_win The window to render the footer content, normally for
 *                 the args of footer_render
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 * \param[in]      hash_id The ID of the hash function to simulate collisions for. This should
 *                 be one of the enum hash_function_ids values defined in hash_config.h.
 */
void
render_hash_collision_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int max_y,
                           int max_x, enum hash_function_ids hash_id) {
    if (content_win == NULL || header_win == NULL || footer_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_hash_collision_page is null");
    }

    curs_set(1); // Show the cursor
    bool nodelay_modified = false;
    if (!is_nodelay(content_win)) {
        nodelay(content_win, TRUE);
        nodelay_modified = true; // Track if we modified nodelay
    }

    // Clear the window before rendering
    werase(content_win);
    wresize(content_win, max_y - BH_LAYOUT_PADDING, max_x);
    mvwin(content_win, 4, 0);
    box(content_win, 0, 0);

    COORD win_size;

    unsigned short title_len = strlen(s_hash_collision_page_title);
    mvwprintw(content_win, 0, (max_x - title_len) / 2, s_hash_collision_page_title);

    hash_config_t current_hash_function = get_hash_config_item(hash_id);

    render_page_details(content_win, current_hash_function, max_x);

    hash_collision_form_init(content_win, max_y, max_x); // Initialize the form fields
    FORM* s_hash_collision_form = hash_collision_form_render(
        content_win, max_y - BH_LAYOUT_PADDING, max_x); // Render the form in the window

    pos_form_cursor(s_hash_collision_form);

    // Initialize result structure
    hash_collision_simulation_result_t* result = malloc(sizeof(hash_collision_simulation_result_t));
    if (!result) {
        render_full_page_error_exit(
            stdscr, 0, 0, "Memory allocation failed for hash collision simulation result.");
    }

    // Initialize result fields
    result->id = hash_id;
    result->attempts_made = -1;
    result->collision_found = false;
    result->collision_input_1 = NULL;
    result->collision_input_2 = NULL;
    result->collision_hash_hex = NULL;

    bool is_done = false;
    int char_input;
    while ((char_input = wgetch(content_win)) != KEY_F(2) && !is_done) {
        hash_form_handle_input(hash_id, char_input,
                               &result); // Handle user input for the form

        if (check_console_window_resize_event(&win_size)) {
            int resize_result = resize_term(win_size.Y, win_size.X);
            if (resize_result != OK) {
                render_full_page_error(
                    content_win, 0, 0,
                    "Unable to resize the UI to the terminal new size. Resize failure.");
            }
            // mvwprintw(stdscr, 0, 0, "%d-%d", win_size.Y, win_size.X); // For debugging purpose only

            wclear(footer_win);

            clear();
            wclear(content_win);
            refresh();

            max_y = win_size.Y;
            max_x = win_size.X;

            wresize(content_win, max_y - BH_LAYOUT_PADDING, max_x);
            box(content_win, 0, 0);
            render_page_details(content_win, current_hash_function, max_x);

            header_render(header_win);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y - 2, max_x);
            hash_collision_form_restore(content_win, max_y, max_x, *result);

            mvwprintw(content_win, 0, (max_x - title_len) / 2, s_hash_collision_page_title);

            wrefresh(content_win);
        }
    }

    hash_collision_form_destroy(); // Clean up the form resources

    curs_set(0); // Hide the cursor
    if (nodelay_modified) {
        nodelay(content_win, FALSE); // Restore nodelay to true
    }

    // Clear the window after user input
    werase(content_win);

    // Refresh the window to show the changes
    wrefresh(content_win);
}