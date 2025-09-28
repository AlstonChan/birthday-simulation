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

#define ACTION_SUBMIT 1

static const char* const s_hash_collision_page_title = "[ Hash Collision Demonstration ]";

static const struct FormButton const s_hash_form_buttons_metadata[] = {
    {"[ Run Simulation ]", "[ Running... ]", ACTION_SUBMIT},
};
static const unsigned short s_hash_form_buttons_metadata_len =
    ARRAY_SIZE(s_hash_form_buttons_metadata);

static const struct FormInputField const s_hash_form_field_metadata[] = {
    {"Max Attempts", 10000, 6}};
static const unsigned short s_hash_form_field_metadata_len = ARRAY_SIZE(s_hash_form_field_metadata);

static form_manager_t* manager = NULL;

// This variable temporarily tracks if the button is highlighted before calling
// update_button_field_is_running to set the button to running state. So this
// is used ONLY in this scope
static bool is_btn_highlighted = false;

/****************************************************************
 INTERNAL FUNCTION
 ****************************************************************/

static FIELD*
hash_collision_form_field_get(int index) {
    return manager->fields[index];
}

/**
 * \brief          Simulates a hash collision using the Birthday Attack algorithm.
 *                 It will first create a hash table with a size based on the maximum number of attempts.
 *                 Then, it will generate random inputs, compute their hashes, and check for collisions.
 *                 If a collision is found, it will store the inputs and the hash in the result structure.
 *                 If no collision is found after the maximum number of attempts, it will return a result
 *                 indicating no collision.
 *
 * \param[in]      max_attempts The maximum number of attempts to find a collision before exiting.
 * \param[in]      thread_pool The thread pool to use for running the hash collision simulation.
 *                 This allows for concurrent execution of the simulation.
 * \param[out]     ctx The context of the birthday attack simulation shared between all worker threads and
 *                 contains the results of the birthday attack reference
 * 
 * \return         hash_collision_simulation_result_t*
 */
static void
hash_collision_simulation_run(unsigned int max_attempts, GThreadPool* thread_pool,
                              hash_collision_context_t* ctx) {
    if (max_attempts <= 0) {
        max_attempts = 10000; // Default to 10,000 attempts for negative or zero attempts
    }

    // The desired table size is 1.3 times the maximum attempts so that
    // the load factor (n / table_size) should ideally stay under 0.75.
    unsigned int desired_table_size = (max_attempts * 1.3);
    unsigned int table_size = next_prime(desired_table_size);
    hash_table_t* table = hash_table_create(table_size);

    if (!table) {
        render_full_page_error_exit(stdscr, 0, 0, "Memory allocation failed for hash table.");
    }

    ctx->table_mutex = g_new0(GMutex, 1);
    ctx->result_mutex = g_new0(GMutex, 1);
    ctx->error_info = error_info_create();
    if (!ctx->error_info) {
       render_full_page_error_exit(stdscr, 0, 0, "Memory allocation failed for ctx error info");
    }

    ctx->shared_table = table;
    ctx->result->attempts_made = 0;
    g_mutex_init(ctx->table_mutex);
    g_mutex_init(ctx->result_mutex);
    ctx->collision_found = false;
    ctx->cancel = 0;
    ctx->remaining_workers = 0;

    // Divide work among threads
    int num_threads = g_thread_pool_get_max_threads(thread_pool);
    unsigned int attempts_per_thread = max_attempts / num_threads;
    unsigned int remaining_attempts = max_attempts % num_threads;

    // Submit work to thread pool
    for (int i = 0; i < num_threads; i++) {
        WorkerData* worker_data = g_new(WorkerData, 1);
        worker_data->ctx = ctx;
        worker_data->attempts_to_make = attempts_per_thread + (i == 0 ? remaining_attempts : 0);
        worker_data->worker_id = i;

        GError* error = NULL;
        g_thread_pool_push(thread_pool, worker_data, &error);

        if (error) {
            g_printerr("Failed to submit work: %s\n", error->message);
            g_error_free(error);
            g_free(worker_data);
        }
    }
}

/**************************************************************
                    FORM HANDLING FUNCTIONS
**************************************************************/

/**
 * \brief          Update the progress bar in the hash collision form sub window.
 *
 * \param[in]      progress The current progress value (number of attempts made).
 * \param[in]      total The total value (maximum number of attempts).
 * \param[in]      is_final Whether this is the final update (true) or an intermediate update (false).
 */
static void
hash_progress_bar_update(int progress, int total, bool is_final) {
    if (progress < 0 || total <= 0 || progress > total) {
        return; // Invalid parameters
    }

    uint8_t starting_y = s_hash_form_field_metadata_len + 1 + 2 + 3 + 1;

    // Clear the row first
    for (int col = BH_FORM_X_PADDING; col <= COLS - BH_FORM_X_PADDING; col++) {
        mvwaddch(manager->sub_win, starting_y, col, ' ');
    }

    int percentage = (progress * 100) / total;

    if (is_final) {
        if (progress >= total) {
            mvwprintw(manager->sub_win, starting_y, BH_FORM_X_PADDING, "Progress: %d%% (%d/%d)",
                      percentage, progress, total);
        } else {
            mvwprintw(manager->sub_win, starting_y, BH_FORM_X_PADDING,
                      "Run %d times (%d%% of total %d)", progress, percentage, total);
        }
    } else {
        mvwprintw(manager->sub_win, starting_y, BH_FORM_X_PADDING, "Progress: %d%% (%d/%d)",
                  percentage, progress, total);
    }
}

/**
 * \brief          Update the exit progress bar in the hash collision form sub window.
 *
 * \param[in]      left The number of threads that have NOT completed.
 * \param[in]      total The total number of threads.
 */
static void
hash_exit_bar_update(int left, int total) {
    uint8_t starting_y = s_hash_form_field_metadata_len + 1 + 2 + 3 + 1;

    // Clear the row first
    for (int col = BH_FORM_X_PADDING; col <= COLS - BH_FORM_X_PADDING; col++) {
        mvwaddch(manager->sub_win, starting_y, col, ' ');
    }

    int percentage = ((total - left) * 100) / total;
    mvwprintw(manager->sub_win, starting_y, BH_FORM_X_PADDING, "Exiting: %d%%", percentage);
}

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

    if (manager->sub_win) {
        delwin(manager->sub_win);
        manager->sub_win = NULL;
    }

    const int sub_win_rows_count = s_hash_form_field_metadata_len + 12;
    const int sub_win_cols_count = max_x - BH_FORM_X_PADDING - BH_FORM_X_PADDING;

    // Create a sub-window for the form with extra space for the button
    manager->sub_win = derwin(win, sub_win_rows_count, sub_win_cols_count, 9, 1);
    keypad(manager->sub_win, TRUE);

    set_form_win(manager->form, win);
    set_form_sub(manager->form, manager->sub_win);

    unpost_form(manager->form); // Safeguard against state issues
    post_form(manager->form);
}

/**
 * \brief          Take the value from the form field as arguments for simulating the
 *                 birthday attack. The result will be stored back to the arguments
 *                 provided to the function.
 *
 * \param[in]      thread_pool The thread pool to use for running the hash collision simulation.
 *                 This allows for concurrent execution of the simulation.
 * \param[out]     ctx The context of the birthday attack simulation shared between all worker threads and
 *                 contains the results of the birthday attack reference
 */
static void
run_hash_collision_from_input(GThreadPool* thread_pool, hash_collision_context_t* ctx) {
    unsigned int attempts = atoi(field_buffer(hash_collision_form_field_get(0), 0));
    return hash_collision_simulation_run(attempts, thread_pool, ctx);
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

    // Clear the sub-window from starting_y to starting_y + 4
    for (unsigned short row = starting_y; row < starting_y + 4; ++row) {
        for (int col = BH_FORM_X_PADDING; col <= COLS - BH_FORM_X_PADDING; col++) {
            mvwaddch(manager->sub_win, row, col, ' ');
        }
    }

    unsigned int attempts = atoi(field_buffer(hash_collision_form_field_get(0), 0));
    if (results.attempts_made < attempts && !results.collision_found) {
        // No results to display
        return;
    }

    if (results.collision_found) {
        // Display the results of the collision simulation
        wattron(manager->sub_win, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
        mvwprintw(manager->sub_win, starting_y, BH_FORM_X_PADDING, "Collision Found at attempt %d!",
                  results.attempts_made);
        wattroff(manager->sub_win, A_BOLD | COLOR_PAIR(BH_SUCCESS_COLOR_PAIR));
        mvwprintw(manager->sub_win, starting_y + 1, BH_FORM_X_PADDING, "Input 1: %s",
                  results.collision_input_1);
        mvwprintw(manager->sub_win, starting_y + 2, BH_FORM_X_PADDING, "Input 2: %s",
                  results.collision_input_2);
        mvwprintw(manager->sub_win, starting_y + 3, BH_FORM_X_PADDING, "Hash   : %s",
                  results.collision_hash_hex);
    } else {
        wattron(manager->sub_win, A_BOLD | COLOR_PAIR(BH_ERROR_COLOR_PAIR));
        mvwprintw(manager->sub_win, starting_y, BH_FORM_X_PADDING,
                  "No Collision Found after %d attempts.", results.attempts_made);
        wattroff(manager->sub_win, A_BOLD | COLOR_PAIR(BH_ERROR_COLOR_PAIR));
    }

    wrefresh(manager->sub_win);
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

    for (unsigned short i = 0; i < s_hash_form_field_metadata_len; ++i) {
        FIELD* field = hash_collision_form_field_get(i);
        int result = form_driver(manager->form, REQ_VALIDATION);

        if (result == E_INVALID_FIELD) {
            display_field_error(manager, field, s_hash_form_field_metadata[i].max_length, true);
            all_valid = false;
        } else {
            clear_field_error(manager, field);
        }
    }

    if (all_valid) {
        set_current_field(manager->form, hash_collision_form_field_get(manager->input_count));
    } else {
        set_current_field(manager->form, hash_collision_form_field_get(0));
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

    if (manager && manager->form != NULL) {
        render_full_page_error_exit(
            win, 0, 0,
            "The hash collision form has already been initialized and another "
            "attempt to initialize is not permitted");
    }

    manager = create_form_manager(s_hash_form_field_metadata, s_hash_form_field_metadata_len,
                                  s_hash_form_buttons_metadata, s_hash_form_buttons_metadata_len);
    if (!manager) {
        render_full_page_error_exit(win, 0, 0,
                                    "Unable to allocate memory for collision form manager");
        return;
    }

    for (unsigned short i = 0; i < manager->input_count; ++i) {
        const struct FormInputField* metadata = &s_hash_form_field_metadata[i];

        manager->fields[i] =
            new_field(1,                             // Field height
                      manager->max_field_length + 1, // Field width
                      i,                             // Field y-position
                      BH_FORM_X_PADDING + BH_FORM_FIELD_BRACKET_PADDING + manager->max_label_length
                          + BH_FORM_FIELD_BRACKET_PADDING, // Field x-position
                      0,                                   // number of offscreen rows
                      0                                    // number of additional working buffers
            );

        if (!manager->fields[i]) {
            // Cleanup on failure
            for (int j = 0; j < i; j++) {
                free_field(manager->fields[j]);
            }
            free(manager->fields);
            free(manager->trackers);
            free(manager);

            render_full_page_error_exit(win, 0, 0,
                                        "Unable to allocate memory for collision form field");
            return;
        }

        // Convert the default value to string
        char* string_buffer = (char*)malloc(sizeof(char) * metadata->max_length + 1);
        if (!string_buffer) {
            render_full_page_error_exit(
                stdscr, 0, 0, "Memory allocation failed for hash collision form default value");
        }

        snprintf(string_buffer, metadata->max_length + 1, "%hu", metadata->default_value);

        // Make the field visible and editable
        field_opts_on(manager->fields[i], O_STATIC);    // Keep field static size
        field_opts_off(manager->fields[i], O_AUTOSKIP); // Don't auto skip to next field
        set_field_back(manager->fields[i],
                       A_NORMAL); // Set normal background for all fields initially
        set_field_buffer(manager->fields[i], 0,
                         string_buffer);                  // Set the default value for the field
        set_field_just(manager->fields[i], JUSTIFY_LEFT); // Left justify the content

        // Set maximum field length
        set_max_field(manager->fields[i], manager->max_field_length);

        // Set the field type to numeric
        int max_value = calculate_form_max_value(metadata->max_length);
        set_field_type(manager->fields[i], TYPE_INTEGER, 0, (long)1, (long)max_value);

        // Initialize tracker
        manager->trackers[i].field = manager->fields[i];
        manager->trackers[i].current_length = strlen(string_buffer);
        manager->trackers[i].max_length = metadata->max_length;
        manager->trackers[i].field_index = i;
        manager->trackers[i].cursor_position = manager->trackers[i].current_length;

        free(string_buffer);
    }

    // Create the submit button field
    manager->fields[manager->input_count] =
        create_button_field(s_hash_form_buttons_metadata[0].label, // Button label
                            s_hash_form_field_metadata_len + 1, BH_FORM_X_PADDING);

    // Create the form
    manager->form = new_form(manager->fields);
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

    if (manager->form == NULL) {
        hash_collision_form_init(win, max_y, max_x); // Initialize the form if not already done
    }

    // Set the label for the field
    for (unsigned short i = 0; i < manager->input_count; ++i) {
        mvwprintw(manager->sub_win, i, BH_FORM_X_PADDING, s_hash_form_field_metadata[i].label);
        mvwprintw(manager->sub_win, i, BH_FORM_X_PADDING + manager->max_label_length, ": [");
        mvwprintw(manager->sub_win, i,
                  BH_FORM_X_PADDING + manager->max_label_length + BH_FORM_FIELD_BRACKET_PADDING + 1
                      + (manager->max_field_length + 1) + BH_FORM_FIELD_BRACKET_PADDING,
                  "]");
    }

    set_current_field(manager->form, manager->fields[0]);
    update_field_highlighting(manager);
    form_driver(manager->form, REQ_END_LINE);

    wrefresh(win);

    return manager->form;
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
    if (!manager->form) {
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
    for (int i = 0; manager->fields[i] != NULL; ++i) {
        const char* buf = field_buffer(manager->fields[i], 0);
        set_field_buffer(manager->fields[i], 0, buf); // Force internal repaint
    }

    // Force redraw current field again
    set_current_field(manager->form, manager->fields[0]);
    form_driver(manager->form, REQ_FIRST_FIELD);

    if (result.attempts_made != -1) {
        render_attack_result(result);
    }

    wrefresh(manager->sub_win);
}

/**
 * \brief          Destroys the hash collision form and frees the memory allocated for it.
 *
 */
static void
hash_collision_form_destroy() {
    free_form_manager(manager);
    manager = NULL;
}

/**
 * \brief          Handles input for the hash collision form.
 *
 * \param[in]      ch The current int character input from the key pressed
 * \param[out]     ctx The context of the birthday attack simulation shared between all worker threads and
 *                 contains the results of the birthday attack reference
 * \param[in]      thread_pool The thread pool to use for running the hash collision simulation.
 *                 This allows for concurrent execution of the simulation.
 */
static void
hash_form_handle_input(int ch, hash_collision_context_t* ctx, GThreadPool* thread_pool) {
    FIELD* active_field = current_field(manager->form);
    int current_index = field_index(active_field);

    int field_max_length = manager->trackers[current_index].max_length;
    int longest_max_length_pad = manager->max_field_length + 1;
    bool is_active_field_input = current_index < manager->input_count;

    switch (ch) {
        case KEY_UP:
        case KEY_DOWN: {
            int result = form_driver(manager->form, REQ_VALIDATION);

            if (result == E_INVALID_FIELD) {
                display_field_error(manager, active_field, field_max_length, true);
            } else {
                clear_field_error(manager, active_field);

                FIELD* old_field = active_field;
                if (ch == KEY_DOWN) {
                    form_driver(manager->form, REQ_NEXT_FIELD);
                } else {
                    form_driver(manager->form, REQ_PREV_FIELD);
                }
                form_driver(manager->form, REQ_END_LINE);

                active_field = current_field(manager->form);
                current_index = field_index(active_field);

                on_field_change(manager, old_field, active_field);
                update_field_highlighting(manager);

                if (current_index < manager->input_count) {
                    is_btn_highlighted = false;
                    pos_form_cursor(manager->form);
                } else {
                    is_btn_highlighted = true;
                    set_field_buffer(hash_collision_form_field_get(manager->input_count), 0,
                                     s_hash_form_buttons_metadata[0].label);
                    pos_form_cursor(manager->form);
                }
            }

        } break;

        case KEY_LEFT:
            bool can_move_left = cursor_can_move_left(manager, active_field);
            if (current_index < manager->input_count && can_move_left) {
                int result = form_driver(manager->form, REQ_PREV_CHAR);
                if (result == E_OK) {
                    unsigned int cursor_pos = get_cursor_position(manager, active_field);
                    if (cursor_pos > 0) {
                        decrement_cursor_position(manager, active_field);
                    }
                }
            }
            break;
        case KEY_RIGHT:
            bool can_move_right = cursor_can_move_right(manager, active_field);
            if (current_index < manager->input_count && can_move_right) {
                int result = form_driver(manager->form, REQ_NEXT_CHAR);
                if (result == E_OK) {
                    unsigned int cursor_pos = get_cursor_position(manager, active_field);
                    if (cursor_pos < get_field_current_length(manager, active_field)) {
                        increment_cursor_position(manager, active_field);
                    }
                }
            }
            break;

        case KEY_BACKSPACE:
        case '\b':
        case 127:
            if (is_active_field_input && get_field_current_length(manager, active_field) > 0) {
                unsigned short prev_length = get_field_length_on_screen(manager, active_field);
                int result = form_driver(manager->form, REQ_DEL_PREV);
                if (result == E_OK) {
                    unsigned short new_length = get_field_length_on_screen(manager, active_field);

                    if (new_length != prev_length) {
                        decrement_field_length(manager, active_field);
                        // Whenever a character is deleted, the cursor position is automatically
                        // move to the left by ncurses, so we need to update our tracker as well
                        decrement_cursor_position(manager, active_field);
                    }
                }
            }
            break;
        case KEY_DC:
            if (is_active_field_input && get_field_current_length(manager, active_field) > 0) {
                unsigned short prev_length = get_field_length_on_screen(manager, active_field);
                int result = form_driver(manager->form, REQ_DEL_CHAR);
                if (result == E_OK) {
                    unsigned short new_length = get_field_length_on_screen(manager, active_field);

                    if (new_length != prev_length) {
                        decrement_field_length(manager, active_field);
                    }
                }
            }
            break;

        case '\n': {
            int result = form_driver(manager->form, REQ_VALIDATION);

            if (result == E_INVALID_FIELD) {
                display_field_error(manager, active_field, field_max_length, true);
            } else if (current_index == manager->input_count) {
                bool is_not_running = g_atomic_int_get(&ctx->result->attempts_made) == -1;

                // We are not going to run another simulation if there are still
                // pending simulation running
                if (is_not_running) {
                    bool all_field_valid = hash_form_validate_all_fields();
                    if (all_field_valid) {
                        run_hash_collision_from_input(thread_pool, ctx);
                    }
                }
            }
        } break;

        default:
            if (is_active_field_input && isdigit(ch)
                && field_has_space_for_char(manager, active_field)) {
                unsigned short prev_length = get_field_length_on_screen(manager, active_field);
                int result = form_driver(manager->form, ch);
                if (result == E_OK) {
                    unsigned short new_length = get_field_length_on_screen(manager, active_field);

                    if (new_length != prev_length) {
                        increment_field_length(manager, active_field);
                        // Whenever a character is added, the cursor position is automatically
                        // move to the right by ncurses, so we need to update our tracker as well
                        increment_cursor_position(manager, active_field);
                    }
                }
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
 * \param[out]     max_y The maximum height of the screen space that can be rendered. The
 *                 value will be updated when a resize happens
 * \param[out]     max_x The maximum width of the screen space that can be rendered. The
 *                 value will be updated when a resize happens
 * \param[in]      hash_id The ID of the hash function to simulate collisions for. This should
 *                 be one of the enum hash_function_ids values defined in hash_config.h.
 * \param[in]      thread_pool The thread pool to use for running the hash collision simulation.
 *                 This allows for concurrent execution of the simulation.
 */
void
render_hash_collision_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int* max_y,
                           int* max_x, enum hash_function_ids hash_id, GThreadPool* thread_pool) {
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
    wresize(content_win, *max_y - BH_LAYOUT_PADDING, *max_x);
    mvwin(content_win, 4, 0);
    box(content_win, 0, 0);

    COORD win_size;

    unsigned short title_len = strlen(s_hash_collision_page_title);
    mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_hash_collision_page_title);

    hash_config_t current_hash_function = get_hash_config_item(hash_id);

    render_page_details(content_win, current_hash_function, *max_x);

    hash_collision_form_init(content_win, *max_y, *max_x); // Initialize the form fields
    FORM* hash_collision_form = hash_collision_form_render(content_win, *max_y - BH_LAYOUT_PADDING,
                                                           *max_x); // Render the form in the window

    pos_form_cursor(hash_collision_form);

    // Initialize result structure
    hash_collision_simulation_result_t* result = malloc(sizeof(hash_collision_simulation_result_t));
    if (!result) {
        render_full_page_error_exit(
            stdscr, 0, 0, "Memory allocation failed for hash collision simulation result.");
    }

    // This prev_result stores the previous result after freeing the content and the result
    // pointer memory. This data is to be used for restoring the form after a resize event
    // only.
    hash_collision_simulation_result_t prev_result;

    // Initialize result fields
    result->attempts_made = -1;
    result->collision_found = false;
    result->collision_input_1 = NULL;
    result->collision_input_2 = NULL;
    result->collision_hash_hex = NULL;

    prev_result.attempts_made = -1;
    prev_result.collision_found = false;
    prev_result.collision_input_1 = NULL;
    prev_result.collision_input_2 = NULL;
    prev_result.collision_hash_hex = NULL;

    // Initialize context state
    hash_collision_context_t* ctx = malloc(sizeof(hash_collision_context_t));
    if (!ctx) {
        render_full_page_error_exit(
            stdscr, 0, 0, "Memory allocation failed for hash collision simulation context.");
    }

    *ctx = (hash_collision_context_t){.hash_id = hash_id,
                                      .shared_table = g_new(hash_table_t, 1),
                                      .table_mutex = g_new(GMutex, 1),
                                      .result_mutex = g_new(GMutex, 1),
                                      .cancel = 0,
                                      .remaining_workers = 0,
                                      .collision_found = g_new(bool, 1),
                                      .result = result,
                                      .error_info = error_info_create()};
   
    if (!ctx->error_info) {
        render_full_page_error_exit(
            stdscr, 0, 0, "Memory allocation failed for hash collision error info.");
    }

    int char_input;

    while (true) {
        char_input = wgetch(content_win);

        if (char_input == KEY_F(2)) {
            g_atomic_int_set((gint*)&ctx->cancel, 1); // Signal cancellation to worker threads
            break;
        }

        hash_form_handle_input(char_input, ctx, thread_pool);

        // If the user has initiated a simulation run, check if the thread pool has
        // finished processing all tasks
        unsigned int max_attempts = atoi(field_buffer(hash_collision_form_field_get(0), 0));
        bool has_results_to_check = g_atomic_int_get(&result->attempts_made) != -1;
        bool all_tasks_completed = g_atomic_int_get(&result->attempts_made) >= max_attempts;
        gint left = g_atomic_int_get((gint*)&ctx->remaining_workers);

        if (left == 0 && has_results_to_check && (all_tasks_completed || result->collision_found || ctx->error_info->has_error)) {
            update_button_field_is_running(
                hash_collision_form_field_get(s_hash_form_field_metadata_len),
                s_hash_form_buttons_metadata[0].label,
                s_hash_form_buttons_metadata[0].loading_label, false);

            if (is_btn_highlighted) {
                update_field_highlighting(manager);

                set_field_buffer(hash_collision_form_field_get(s_hash_form_field_metadata_len), 0,
                                 s_hash_form_buttons_metadata[0].label);
                pos_form_cursor(manager->form);
            }

            if (ctx->error_info->has_error) {
                char result[512];
                snprintf(result, sizeof(result), "%s%s\n%s%s%s%s%s", 
                    "An error had occured when calculating hashes.\nError: ",
                    ctx->error_info->error_message,
                    " at ",
                    ctx->error_info->error_location,
                    " (", error_type_to_string(ctx->error_info->error_type)," )"
                );
                render_full_page_error(content_win, 0, 0, result);
            }

            hash_progress_bar_update(result->attempts_made, max_attempts, true);
            render_attack_result(*ctx->result);
            deep_copy_hash_collision_simulation_result(&prev_result, result);
            clear_result_hash_collision_context(ctx, false);
        } else if (has_results_to_check) {
            // if button is not in running state, set it to running state
            if (strcmp(
                    field_buffer(hash_collision_form_field_get(s_hash_form_field_metadata_len), 0),
                    s_hash_form_buttons_metadata[0].loading_label)
                != 0) {
                update_button_field_is_running(
                    hash_collision_form_field_get(s_hash_form_field_metadata_len),
                    s_hash_form_buttons_metadata[0].label,
                    s_hash_form_buttons_metadata[0].loading_label, true);
            }

            // Update the intermediate result display
            hash_progress_bar_update(result->attempts_made, max_attempts, false);
            wrefresh(manager->sub_win);
        }

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

            *max_y = win_size.Y;
            *max_x = win_size.X;

            wresize(content_win, *max_y - BH_LAYOUT_PADDING, *max_x);
            box(content_win, 0, 0);
            render_page_details(content_win, current_hash_function, *max_x);

            header_render(header_win);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y - 2, *max_x);
            hash_collision_form_restore(content_win, *max_y, *max_x, prev_result);
            hash_progress_bar_update(prev_result.attempts_made, max_attempts, true);

            mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_hash_collision_page_title);
            wrefresh(content_win);
        }
    }

    bool waiting_all_threads_to_exit = true;
    while (waiting_all_threads_to_exit) {
        gint left = g_atomic_int_get((gint*)&ctx->remaining_workers);
        hash_exit_bar_update(left, g_thread_pool_get_max_threads(thread_pool));

        if (left == 0) {
            waiting_all_threads_to_exit = false;
        }
    }

    // Cleanup
    clear_result_hash_collision_context(ctx, true);
    clear_result_hash_collision_simulation_result(&prev_result, false);
    hash_collision_form_destroy();

    curs_set(0); // Hide the cursor
    if (nodelay_modified) {
        nodelay(content_win, FALSE); // Restore nodelay to true
    }

    // Clear the window after user input
    werase(content_win);

    // Refresh the window to show the changes
    wrefresh(content_win);
}