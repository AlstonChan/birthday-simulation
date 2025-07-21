#ifndef PARADOX_MATH_H
#define PARADOX_MATH_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Calculates the probability of at least two people having the same birthday
 *        in a group of people (Birthday Paradox/Problem)
 *
 * @param domain_size The number of possible birthdays (typically 365 for non-leap years)
 * @param sample_size The number of people in the group
 * @return double The probability (between 0.0 and 1.0) of at least one birthday collision
 *
 * @note The calculation uses the complement approach:
 *       P(collision) = 1 - P(no collision)
 */
double calculate_birthday_collision_probability(int domain_size, int sample_size);

/**
 * @brief Simulates the birthday paradox by running multiple random trials
 *
 * @param domain_size The range of possible values (e.g., 365 for days in a year)
 * @param sample_size The number of random values to generate per trial (e.g., number of people)
 * @param num_runs The number of simulation trials to run
 * @return double The percentage of trials where a collision was found (0.0 to 100.0)
 *
 * @note This function uses actual random sampling to empirically test
 *       the birthday paradox probability through simulation
 */
double simulate_birthday_collision(int domain_size, int sample_size, int num_runs);

#endif /* PARADOX_MATH_H */
