/**
 * \file            explanation.c
 * \brief           A documentation page for both the application and the core
 *                  theme of this application -- birthday paradox and birthday
 *                  attack.
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "explanation.h"

static const char const* s_explanation_page_title = "[ Birthday Paradox Information ]";

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Read the file content given a pointer to the file and save the file
 *                 content in lines in an array.
 *
 * \param[in]      ptr_file The file pointer of the FILE to read the lines from
 * \param[out]     ptr_line_count The number of lines read from the file
 * \return         The array of string if the successfully read all the lines, else
 *                 return false
 */
char**
load_all_lines(FILE* ptr_file, size_t* ptr_line_count) {
    // Buffer for storing a line
    char line[1024];
    // The number of line that can be stored
    size_t capacity = 100;
    // The current number of lines stored
    size_t count = 0;

    char** lines = malloc(capacity * sizeof(char*));
    if (!lines) {
        return NULL;
    }

    while (fgets(line, sizeof(line), ptr_file)) {
        size_t len = strlen(line);

        // Expand storage if the count is exceeding the capacity
        if (count >= capacity) {
            capacity *= 2;
            char** new_lines = realloc(lines, capacity * sizeof(char*));
            if (!new_lines) {
                return NULL;
            }
            lines = new_lines;
        }

        lines[count] = malloc(len + 1);
        if (!lines[count]) {
            break;
        }
        strcpy(lines[count], line);
        count++;
    }

    *ptr_line_count = count;
    return lines;
}

/**
 * \brief          Parse and render the a string's of content into the pad window
 *                 with word wrapping and add style as markdown text
 *
 * \param[in]      pad The pad window to render the line
 * \param[in]      line The string to render
 * \return         True if the lines can be rendered correctly, false otherwise
 */
static bool
render_line(WINDOW* pad, const char* line) {
    char* buffer = malloc(strlen(line) + 1);
    if (!buffer) {
        return false;
    }
    strcpy(buffer, line);

    int pad_width = getmaxx(pad);

    char* word = strtok(buffer, " ");
    int char_x_pos = 0;

    bool in_bold = false;
    bool in_italic = false;

    while (word != NULL) {
        int word_len = strlen(word);

        // Wrap if needed
        // The last word character position + word length + a single space if needed
        // longer than the pad width -> warp
        if (char_x_pos + (char_x_pos > 0 ? 1 : 0) + word_len >= pad_width) {
            char_x_pos = 0;
            waddch(pad, '\n');
        }

        // Add space before word if not first word in line
        if (char_x_pos > 0) {
            waddch(pad, ' ');
            char_x_pos++;
        }

        // Add each character of the word
        for (int i = 0; i < word_len;) {
            // Handle **bold**
            if (word[i] == '*' && word[i + 1] == '*') {
                in_bold = !in_bold;
                if (in_bold) {
                    wattron(pad, A_BOLD);
                } else {
                    wattroff(pad, A_BOLD);
                }
                i += 2;
                continue;
            }

            // Handle _italic_
            if (word[i] == '_') {
                in_italic = !in_italic;
                if (in_italic) {
                    wattron(pad, A_UNDERLINE);
                } else {
                    wattroff(pad, A_UNDERLINE);
                }
                i++;
                continue;
            }

            waddch(pad, word[i]);
            char_x_pos++;
            i++;
        }

        word = strtok(NULL, " ");
    }

    free(buffer);
    return true;
}

/****************************************************************
                       EXTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Renders the explanation page in the given window.
 *
 * \param[in]      content_win The window to render the explanation page on
 * \param[in]      header_win The window to render the header content, normally for
 *                 the args of header_render
 * \param[in]      footer_win The window to render the footer content, normally for
 *                 the args of footer_render
 * \param[out]     max_y The maximum height of the screen space that can be rendered. The
 *                 value will be updated when a resize happens
 * \param[out]     max_x The maximum width of the screen space that can be rendered. The
 *                 value will be updated when a resize happens
 */
void
render_explanation_page(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int* max_y,
                        int* max_x) {
    if (content_win == NULL || header_win == NULL || footer_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0,
                                    "The window passed to render_explanation_page is null");
    }

    COORD win_size;
    FILE* ptr_content_file;

    if ((ptr_content_file = fopen("./src/explanation.md", "r")) == NULL) {
        render_full_page_error_exit(content_win, 0, 0,
                                    "The application failed to load the page content");
    }

    WINDOW* content_pad = newpad(5000, *max_x - BH_FORM_X_PADDING - BH_FORM_X_PADDING);
    mvwin(content_pad, BH_LAYOUT_PADDING, BH_FORM_X_PADDING);

    bool nodelay_modified = false;
    if (!is_nodelay(content_win)) {
        nodelay(content_win, TRUE);
        nodelay_modified = true; // Track if we modified nodelay
    }

    // Be aware that a single line can only ever holds 1024 character
    size_t total_lines = 0;
    char** all_lines = load_all_lines(ptr_content_file, &total_lines);
    if (all_lines == NULL) {
        render_full_page_error_exit(content_win, 0, 0,
                                    "Memory allocation failed when loading the page content");
    }
    fclose(ptr_content_file);

    for (size_t i = 0; i < total_lines; ++i) {
        if (!render_line(content_pad, all_lines[i])) {
            render_full_page_error_exit(content_win, 0, 0,
                                        "Unable to render the content line correctly");
        }
    }

    // Clear the window before rendering
    werase(content_win);

    // Resize the window for the explanation page
    wresize(content_win, *max_y - BH_LAYOUT_PADDING, *max_x);

    // Center the explanation page window
    mvwin(content_win, 4, 0);
    box(content_win, 0, 0);

    unsigned short title_len = strlen(s_explanation_page_title);
    mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_explanation_page_title);

    // Refresh the window to show the explanation page
    wrefresh(content_win);

    int pad_y = 0;
    prefresh(content_pad, pad_y, 0, BH_LAYOUT_PADDING, BH_FORM_X_PADDING,
             *max_y - BH_LAYOUT_PADDING, *max_x - BH_FORM_X_PADDING);

    int ch;
    while ((ch = wgetch(content_win)) != KEY_F(2)) {
        switch (ch) {
            case KEY_DOWN:
                if (pad_y < *max_y - BH_LAYOUT_PADDING) {
                    pad_y++;
                }
                break;
            case KEY_UP:
                if (pad_y > 0) {
                    pad_y--;
                }
                break;
        }

        // Render pad into the defined region
        prefresh(content_pad, pad_y, 0, BH_LAYOUT_PADDING, BH_FORM_X_PADDING,
                 *max_y - BH_LAYOUT_PADDING, *max_x - BH_FORM_X_PADDING);

        if (check_console_window_resize_event(&win_size)) {
            int resize_result = resize_term(win_size.Y, win_size.X);
            if (resize_result != OK) {
                render_full_page_error(
                    content_win, 0, 0,
                    "Unable to resize the UI to the terminal new size. Resize failure.");
            }
            // mvwprintw(stdscr, 0, 0, "%d-%d", win_size.Y, win_size.X); // For debugging purpose only
            clear();
            wclear(content_win);
            wclear(footer_win);

            refresh();

            *max_y = win_size.Y;
            *max_x = win_size.X;

            // No way to resize the pad, so just delete the old pad and recreate it with a new size
            delwin(content_pad);
            content_pad = newpad(5000, *max_x - BH_FORM_X_PADDING - BH_FORM_X_PADDING);

            wresize(content_win, *max_y - BH_LAYOUT_PADDING, *max_x);

            box(content_win, 0, 0);
            mvwprintw(content_win, 0, (*max_x - title_len) / 2, s_explanation_page_title);

            header_render(header_win);
            mvwin(footer_win, win_size.Y - 2, 0);
            footer_render(footer_win, win_size.Y - 2, *max_x);

            for (size_t i = 0; i < total_lines; ++i) {
                if (!render_line(content_pad, all_lines[i])) {
                    render_full_page_error_exit(content_win, 0, 0,
                                                "Unable to render the content line correctly");
                }
            }

            wrefresh(content_win);
        }
    }

    if (nodelay_modified) {
        nodelay(content_win, FALSE); // Restore nodelay to true
    }

    werase(content_win);
    wrefresh(content_win);

    for (size_t i = 0; i < total_lines; ++i) {
        free(all_lines[i]);
    }
    free(all_lines);
}