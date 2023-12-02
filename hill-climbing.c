//
// Created by Mael Kerichard on 02/12/2023.
//
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "constants.c"

// Function to calculate the sum of the current subset
unsigned long calculateSum(const SSP *instance) {
    unsigned long sum = 0;
    for (int i = 0; i < instance->n; i++) {
        if (instance->solution[i] == 1) {
            sum += instance->set[i];
        }
    }
    return sum;
}

// Hill Climbing function to find a subset sum
HillClimbingResult hillClimbing(const SSP *instance) {
    clock_t end;
    const clock_t start = clock();  // Start timing

    HillClimbingResult result;
    int i;

    for (int restart = 0; restart < RESTART; restart++) {
        // Randomly initializing solution
        for (i = 0; i < instance->n; i++) {
            instance->solution[i] = rand() % 2;
        }

        for (i = 0; i < MAX_ITERATION; i++) {
            const unsigned long currentSum = calculateSum(instance);

            // Check if we have found a solution
            if (currentSum == instance->target) {
                // printf("Solution found in restart %d, iteration %d\n", restart, i);
                end = clock();
                result.execTime = ((double) (end - start)) / CLOCKS_PER_SEC;
                result.solutionFound = 1;
                return result;
            }

            // Randomly tweak a bit in the solution
            int randIndex = rand() % instance->n;
            instance->solution[randIndex] = !instance->solution[randIndex];
        }


    }
    end = clock();
    result.execTime = ((double) (end - start)) / CLOCKS_PER_SEC;
    result.solutionFound = 0;
    return result;
}
