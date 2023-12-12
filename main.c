
/*
 * Subset-Sum Problem (SSP)
 *
 * ESIR HPC project 2023
 *
 * last update: November 18th, 2023
 *
 * AM
 */

#include <stdlib.h>
#include "benchmark.c"

// main
int main() {
 srand(666);  // Seed the random number generator
 benchmarkHillClimbing();
 benchmarkHillClimbingDensity();
 return 0;
}
