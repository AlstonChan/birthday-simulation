#ifndef PARADOX_MATH_H
#define PARADOX_MATH_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

double calculate_birthday_collision_probability(int domain_size, int sample_size);

double simulate_birthday_collision(int domain_size, int sample_size, int num_runs);

#endif
