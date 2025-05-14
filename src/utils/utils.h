#ifndef UTILS_H
#define UTILS_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void print_in_middle(WINDOW *win, unsigned int start_y, unsigned int start_x, unsigned int width,
                     const char *string, chtype color);

#endif