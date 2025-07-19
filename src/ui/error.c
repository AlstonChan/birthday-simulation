#include "error.h"

void _full_page_error(WINDOW *win, int max_y, int max_x, char *message) {
  if (win == NULL)
    win = stdscr;

  if (max_y == 0) {
    max_y = getmaxy(win);
  }

  if (max_x == 0) {
    max_x = getmaxx(win);
  }

  wclear(win);
  wattron(win, COLOR_PAIR(BH_ERROR_COLOR_PAIR));
  box(win, 0, 0);

  // Word wrapping
  char *msg = strdup(message); // Duplicate message so we can modify it
  char *lines[100];
  int line_count = 0;
  char *p = msg;

  while (*p != '\0') {
    char *line_start = p;
    int len = 0;
    char *last_space = NULL;

    while (*p != '\0' && len < MAX_LINE_WIDTH) {
      if (*p == ' ')
        last_space = p;
      p++;
      len++;
    }

    if (*p != '\0' && last_space != NULL) {
      // Wrap at last space
      *last_space = '\0';
      p = last_space + 1;
    }

    lines[line_count++] = line_start;
  }

  // Centered vertical position
  int y_start = (max_y - line_count) / 2;

  // Render the actual message content with word wrapping
  for (int i = 0; i < line_count; i++) {
    int msg_len = strlen(lines[i]);
    int x = (max_x - msg_len) / 2;
    mvwprintw(win, y_start + 2 + i, x, "%s", lines[i]);
  }

  // Render the error title
  wattron(win, A_BOLD | A_UNDERLINE);
  const char *error_title = "An Application Error Had Occurred!";
  mvwprintw(win, y_start, (max_x - strlen(error_title)) / 2, "%s", error_title);
  wattroff(win, A_BOLD | A_UNDERLINE);

  // Render the exit error screen message
  const char *exit_message = "Press F1 to exit";
  mvwprintw(
      win, y_start + 2 + line_count + 2, (max_x - strlen(exit_message)) / 2, "%s", exit_message);

  wattroff(win, COLOR_PAIR(BH_ERROR_COLOR_PAIR));
  free(msg);
  wrefresh(win);

  int char_input;
  while ((char_input = wgetch(win)) != KEY_F(1)) {
  }
}

void render_full_page_error(WINDOW *win, int max_y, int max_x, char *message) {
  _full_page_error(win, max_y, max_x, message);
}

void render_full_page_error_exit(WINDOW *win, int max_y, int max_x, char *message) {
  _full_page_error(win, max_y, max_x, message);
  exit(1);
}