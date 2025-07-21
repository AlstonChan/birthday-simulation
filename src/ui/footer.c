#include "footer.h"

/**
 * \brief          Render the footer of the application
 *
 * \param[in]      footer_win The window to render the content at.
 * \param[in]      max_y The maximum height of the screen space that can be rendered
 * \param[in]      max_x The maximum width of the screen space that can be rendered
 */
void
footer_render(WINDOW* footer_win, int max_y, int max_x) {
    if (footer_win == NULL) {
        render_full_page_error_exit(stdscr, 0, 0, "The window passed to footer_render is null");
    }

    if (max_y == 0) {
        max_y = getmaxy(footer_win);
    }

    if (max_x == 0) {
        max_x = getmaxx(footer_win);
    }

    static const char const* license_text = "2025 Chan Alston - MPL 2.0";
    const unsigned short license_text_len = strlen(license_text);

    wattron(footer_win, A_UNDERLINE);
    mvwprintw(footer_win, 1, (max_x - license_text_len) / 2, license_text);
    wattroff(footer_win, A_UNDERLINE);

    wrefresh(footer_win);
}