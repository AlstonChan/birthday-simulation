#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <ncurses/ncurses.h>
#include <openssl/crypto.h>
#include <windows.h>

#include "../ui/error.h"
#include "../ui/footer.h"
#include "../ui/header.h"
#include "../utils/resize.h"
#include "version.h"

void render_system_info(WINDOW* content_win, WINDOW* header_win, WINDOW* footer_win, int max_y,
                        int max_x);

#endif