//
// Created by Mael Kerichard on 02/12/2023.
//

#include "utils.c"
#include "hill-climbing.c"
#include "hill-climbing-parallel.c"

void benchmarkHillClimbing() {
    const int n_values[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
    int numTests = sizeof(n_values) / sizeof(n_values[0]);
    SSP instance;

    printf("| n | Density | Execution Time (Regular) | Solution Found (Regular) | Execution Time (OpenMP) | Solution Found (OpenMP) |\n");
    printf("|---|---------|------------------------|------------------------|------------------------|------------------------|\n");

    for (int i = 0; i < numTests; i++) {
        const double density = genSSP(n_values[i], &instance, 100);
        const HillClimbingResult result = hillClimbing(&instance);
        const HillClimbingResult resultOpenMP = hillClimbingOpenMP(&instance);
        printf("| %d | %lf | %f seconds | %s | %f seconds | %s |\n", n_values[i], density, result.execTime, result.solutionFound ? "Yes" : "No", resultOpenMP.execTime, resultOpenMP.solutionFound ? "Yes" : "No");
        freeSSP(&instance);
    }
}

void benchmarkHillClimbingDensity() {
    const double density_values[] = {2, 3, 4, 5, 10, 100, 1000, 10000, 100000, 1000000  };
    int numTests = sizeof(density_values) / sizeof(density_values[0]);
    SSP instance;
    const int n = 500; // Fixed n value

    printf("| Density | Execution Time (Regular) | Solution Found (Regular) | Execution Time (OpenMP) | Solution Found (OpenMP) |\n");
    printf("|---------|------------------------|------------------------|------------------------|------------------------|\n");

    for (int i = 0; i < numTests; i++) {
        const double density = genSSP(n, &instance, density_values[i]);
        const HillClimbingResult result = hillClimbing(&instance);
        const HillClimbingResult resultOpenMP = hillClimbingOpenMP(&instance);
        printf("| %lf | %f seconds | %s | %f seconds | %s |\n", density, result.execTime, result.solutionFound ? "Yes" : "No", resultOpenMP.execTime, resultOpenMP.solutionFound ? "Yes" : "No");
        freeSSP(&instance);
    }
}