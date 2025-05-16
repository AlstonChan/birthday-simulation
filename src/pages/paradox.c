#include <ncurses/form.h>
#include <ncurses/ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "paradox.h"

/**
 * @brief The title of the paradox page.
 *
 */
static const char const *paradox_page_title = "[ Birthday Paradox Simulation ]";

/**
 * @brief The structure for the input fields in the paradox form.
 *
 */
static const struct ParadoxInputField paradox_fields[] = {{"Domain Size (days)", 365, 3},
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

  // Allocate memory for the form fields
  paradox_field = (FIELD **)calloc((size_t)(paradox_fields_len + 1), sizeof(FIELD *));

  // Find the longest label length for the fields
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    unsigned short label_length = strlen(paradox_fields[i].label);
    if (label_length > max_label_length)
      max_label_length = label_length;
  }

  // Create form fields for each paradox field
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    paradox_field[i] = new_field(1, 30, i + 2, max_label_length + 4, 0, 0);

    // Convert the default value to string
    char *string_buffer;
    string_buffer = (char *)malloc(sizeof(char) * paradox_fields[i].max_length);
    snprintf(string_buffer, sizeof(string_buffer), "%hu", paradox_fields[i].default_value);

    set_field_buffer(paradox_field[i], 0, string_buffer); // Set the default value for the field

    // Make the field visible on screen, display data during field entry
    set_field_opts(paradox_field[i], O_VISIBLE | O_PUBLIC);
    field_opts_off(paradox_field[i], O_AUTOSKIP); // does not autoskip when the field is full
  }

  // Add NULL terminator to the end of the array
  paradox_field[paradox_fields_len] = NULL;

  // Create a sub-window for the form
  paradox_form_sub_win = derwin(win, paradox_fields_len + 2, COLS - 4, 1, 1);

  // Create the form
  paradox_form = new_form(paradox_field);           // Create the form with the fields
  set_form_win(paradox_form, win);                  // Set the main window for the form
  set_form_sub(paradox_form, paradox_form_sub_win); // Set the sub-window for the form
  post_form(paradox_form);                          // Post the form to the window

  set_current_field(paradox_form, paradox_field[0]); // Set the first field as current
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
void paradox_form_render(WINDOW *win, int max_y, int max_x) {
  if (win == NULL)
    return; // If no window is provided, do nothing

  if (paradox_form == NULL)
    paradox_form_init(win); // Initialize the form if not already done

  // Set the label for the field
  for (unsigned short i = 0; i < paradox_fields_len; ++i) {
    mvwprintw(paradox_form_sub_win, i + 2, 2, paradox_fields[i].label);
    mvwprintw(paradox_form_sub_win, i + 2, max_label_length + 2, ": ");
  }

  wrefresh(win);
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

  // Clear the window before rendering
  werase(win);

  // Resize the window for the paradox page
  wresize(win, max_y - 6, max_x);

  // Center the paradox page window
  mvwin(win, 4, 0);

  box(win, 0, 0);

  unsigned short title_len = strlen(paradox_page_title);
  mvwprintw(win, 0, (max_x - title_len) / 2, paradox_page_title);

  paradox_form_init(win);                 // Initialize the paradox form
  paradox_form_render(win, max_y, max_x); // Render the paradox form

  // Refresh the window to show the paradox page
  wrefresh(win);

  // Wait for user input before closing the system info window
  wgetch(win);

  // Clear the window after user input
  werase(win);

  // Refresh the window to show the changes
  wrefresh(win);
}