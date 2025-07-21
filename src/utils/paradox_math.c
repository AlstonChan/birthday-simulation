/**
 * \file            paradox_math.c
 * \brief           The logic and math behind the birthday paradox simulation
 *                  that calculate the probability for the birthday paradox
 */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "paradox_math.h"

/****************************************************************
                       INTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Helper function to check if there's a collision in 
 *                 an array of numbers
 *
 * \param[in]      numbers An array of numbers that will be checked if
 *                 any of the two number is the same
 * \param[in]      size The size of the array to be checked
 * \return         True if a collision is found, and false if no collisions
 *                 occurred
 * 
 */
static bool
has_collision(const int* numbers, int size) {
    // Check each pair of numbers for a match
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (numbers[i] == numbers[j]) {
                return true;
            }
        }
    }
    return false;
}

/****************************************************************
                       EXTERNAL FUNCTION
****************************************************************/

/**
 * \brief          Calculates the probability of at least two people 
 *                 having the same birthday in a group of people
 *                 (Birthday Paradox/Problem)
 *
 * \param[in]      domain_size The number of possible birthdays
 *                 (typically 365 for non-leap years)
 * \param[in]      sample_size The number of people in the group
 * \return         The probability (between 0.0 and 1.0) of 
 *                 at least one birthday collision
 *
 * \note           The calculation uses the complement approach:
 *                 P(collision) = 1 - P(no collision)
 */
double
calculate_birthday_collision_probability(int domain_size, int sample_size) {
    // Handle edge cases
    if (sample_size <= 1) {
        return 0.0; // No collision possible with 0 or 1 person
    }
    if (sample_size > domain_size) {
        return 1.0; // Guaranteed collision by pigeonhole principle
    }

    // We calculate the probability of NO collision occurring and then subtract from 1
    // This is easier than calculating collision probability directly

    // Start with probability of 1.0 (100%)
    double no_collision_prob = 1.0;

    // For each person after the first person
    for (int i = 1; i < sample_size; i++) {
        // Multiply by the probability that the current person doesn't share
        // a birthday with any previous person
        // For each new person, they must avoid (i) days already "taken"
        // This is (N - i) / N
        // Example:
        // i = 0: (N - 0) / N = N / N = 1 (first person can have any birthday)
        // i = 1: (N - 1) / N (second person's birthday is different from the first)
        // i = 2: (N - 2) / N (third person's birthday is different from the first two)
        // ...
        // i = k-1: (N - (k-1)) / N (k-th person's birthday is different from the previous k-1)
        no_collision_prob *= (double)(domain_size - i) / domain_size;
    }

    // The probability of at least one collision is the complement of no collision
    return 1.0 - no_collision_prob;
}

/**
 * \brief          Simulates the birthday paradox by running multiple
 *                 random trials.
 *
 * \param[in]      domain_size The range of possible values (e.g., 365 
 *                 for days in a year)
 * \param[in]      sample_size The number of random values to generate per
 *                 trial (e.g., number of people)
 * \param[in]      num_runs The number of simulation trials to run
 * \return         The percentage of trials where a collision was
 *                 found (0.0 to 100.0). If memory allocation fails, -1.0
 *                 will be returned
 *
 * \note           This function uses actual random sampling to empirically test
 *                 the birthday paradox probability through simulation
 */
double
simulate_birthday_collision(int domain_size, int sample_size, int num_runs) {
    // Seed the random number generator if it hasn't been seeded
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    // Allocate array for storing random numbers in each trial
    int* numbers = (int*)malloc(sample_size * sizeof(int));
    if (!numbers) {
        return -1.0; // Memory allocation failed
    }

    int collisions_found = 0;

    // Run the simulation num_runs times
    for (int run = 0; run < num_runs; run++) {
        // Generate random numbers for this trial
        for (int i = 0; i < sample_size; i++) {
            // Generate number in range [1, domain_size]
            numbers[i] = (rand() % domain_size) + 1;
        }

        // Check if this trial had a collision
        if (has_collision(numbers, sample_size)) {
            collisions_found++;
        }
    }

    // Free the allocated memory
    free(numbers);

    // Calculate and return the percentage
    return (100.0 * collisions_found) / num_runs;
}
