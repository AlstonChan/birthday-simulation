#include "paradox_math.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

double calculate_birthday_collision_probability(int domain_size, int sample_size) {
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
 * Helper function to check if there's a collision in an array of numbers
 */
static bool has_collision(const int *numbers, int size) {
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

double simulate_birthday_collision(int domain_size, int sample_size, int num_runs) {
  // Seed the random number generator if it hasn't been seeded
  static bool seeded = false;
  if (!seeded) {
    srand((unsigned int)time(NULL));
    seeded = true;
  }

  // Allocate array for storing random numbers in each trial
  int *numbers = (int *)malloc(sample_size * sizeof(int));
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
