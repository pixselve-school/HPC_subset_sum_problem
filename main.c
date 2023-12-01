
/*
 * Subset-Sum Problem (SSP)
 *
 * ESIR HPC project 2023
 *
 * last update: November 18th, 2023
 *
 * AM
 */
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils.c"

// Function to compute the sum of a given solution
unsigned long computeSum(SSP instance, unsigned int* solution) {
    unsigned long sum = 0;
    for (unsigned int i = 0; i < instance.n; i++) {
        if (solution[i] == 1) {
            sum += instance.set[i];
        }
    }
    return sum;
}

// Function to copy a solution
void copySolution(unsigned int* dest, unsigned int* src, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

// Random Restart Hill-Climbing algorithm for SSP
bool randomRestartHillClimbingSSP(SSP instance) {
    unsigned int* currentSolution = (unsigned int*)calloc(instance.n, sizeof(unsigned int));
    unsigned int* neighborSolution = (unsigned int*)calloc(instance.n, sizeof(unsigned int));
    unsigned int* bestSolution = (unsigned int*)calloc(instance.n, sizeof(unsigned int));
    unsigned long currentSum, neighborSum, bestSum = ULONG_MAX;
    int maxIterationsPerRestart = 10000, maxRestarts = 1000;
    int restart, iteration, improved;

    srand(time(NULL));

    for (restart = 0; restart < maxRestarts; restart++) {
        // Initialize a random solution
        for (unsigned int i = 0; i < instance.n; i++) {
            currentSolution[i] = rand() % 2;
        }

        improved = 1;
        for (iteration = 0; iteration < maxIterationsPerRestart && improved; iteration++) {
            improved = 0;

            // Copy current solution to neighbor
            copySolution(neighborSolution, currentSolution, instance.n);

            // Modify the neighbor solution
            neighborSolution[rand() % instance.n] ^= 1; // Flip a random bit

            // Evaluate solutions
            currentSum = computeSum(instance, currentSolution);
            neighborSum = computeSum(instance, neighborSolution);

            // Check if neighbor is closer to the target
            if (abs((int)(instance.target - neighborSum)) < abs((int)(instance.target - currentSum))) {
                improved = 1;
                copySolution(currentSolution, neighborSolution, instance.n); // Update current solution
            }
        }

        currentSum = computeSum(instance, currentSolution);
        if (abs((int)(instance.target - currentSum)) < abs((int)(instance.target - bestSum))) {
            bestSum = currentSum;
            copySolution(bestSolution, currentSolution, instance.n);
        }
    }
    free(currentSolution);
    free(neighborSolution);
    free(bestSolution);
    return bestSum == instance.target;
}
// Random Restart Hill-Climbing algorithm with OpenMP
bool randomRestartHillClimbingSSP_OpenMP(SSP instance) {
    auto* bestSolution = (unsigned int*)calloc(instance.n, sizeof(unsigned int));
    unsigned long bestSum = ULONG_MAX;
    int maxIterationsPerRestart = 100000, maxRestarts = 10000;
    #pragma omp parallel
    {
        auto* currentSolution = (unsigned int*)calloc(instance.n, sizeof(unsigned int));
        auto* neighborSolution = (unsigned int*)calloc(instance.n, sizeof(unsigned int));
        unsigned long currentSum, neighborSum;
        int improved, iteration;
        #pragma omp for
        for (int restart = 0; restart < maxRestarts; restart++) {
            // Initialize a random solution
            for (unsigned int i = 0; i < instance.n; i++) {
                currentSolution[i] = rand() % 2;
            }

            improved = 1;
            for (iteration = 0; iteration < maxIterationsPerRestart && improved; iteration++) {
                improved = 0;

                // Copy current solution to neighbor
                copySolution(neighborSolution, currentSolution, instance.n);

                // Modify the neighbor solution
                neighborSolution[rand() % instance.n] ^= 1; // Flip a random bit

                // Evaluate solutions
                currentSum = computeSum(instance, currentSolution);
                neighborSum = computeSum(instance, neighborSolution);

                // Check if neighbor is closer to the target
                if (abs((int)(instance.target - neighborSum)) < abs((int)(instance.target - currentSum))) {
                    improved = 1;
                    copySolution(currentSolution, neighborSolution, instance.n); // Update current solution
                }
            }

            currentSum = computeSum(instance, currentSolution);
            #pragma omp critical
            {
                if (abs((int)(instance.target - currentSum)) < abs((int)(instance.target - bestSum))) {
                    bestSum = currentSum;
                    copySolution(bestSolution, currentSolution, instance.n);
                }
            }
        }

        free(currentSolution);
        free(neighborSolution);
    }
    return bestSum == instance.target;

    free(bestSolution);
}

// Benchmarking function
void benchmark(SSP instance, bool (*algorithm)(SSP), const char* algorithmName) {
    double startTime, endTime;

    // printf("Benchmarking %s...\n", algorithmName);
    startTime = omp_get_wtime();
    bool result = algorithm(instance);
    endTime = omp_get_wtime();

    // printf("%s took %lf seconds.\n", algorithmName, endTime - startTime);
    double time = endTime - startTime;
    printf("%f", time);
    if (result) {
        printf(" (YES)");
    } else {
        printf(" (NO)");
    }
    printf(" |");

}

// main function
int main() {
    int n_values[] = {1000, 2000, 3000, 4000, 5000}; // Different values of n
    int num_n_values = sizeof(n_values) / sizeof(n_values[0]);

    printf("| n  | Density | Non-Parallel (s) | Parallel (s) |\n");
    printf("|----|--|------------------|--------------|\n");

    for (int i = 0; i < num_n_values; i++) {
        int n = n_values[i];
        SSP instance;
        double density = genSSP(n, &instance, 100);

        printf("| %d | %f | ", n, density);

        // Benchmark non-parallel version
        benchmark(instance, randomRestartHillClimbingSSP, "Non-Parallel");

        // Benchmark parallel version
        benchmark(instance, randomRestartHillClimbingSSP_OpenMP, "Parallel");

        printf("\n");
        freeSSP(&instance);
    }

    return 0;
}