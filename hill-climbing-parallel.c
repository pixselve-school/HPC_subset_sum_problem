//
// Created by Mael Kerichard on 02/12/2023.
//

#include <float.h>
#include <stdlib.h>
#include <time.h>
#include "constants.c"

HillClimbingResult hillClimbingOpenMP(const SSP *instance) {
    HillClimbingResult bestResult;
    bestResult.execTime = DBL_MAX;
    bestResult.solutionFound = 0;

#pragma omp parallel
    {
        HillClimbingResult result;
        result.execTime = DBL_MAX;
        result.solutionFound = 0;

#pragma omp for
        for (int restart = 0; restart < RESTART; restart++) {
            const clock_t start = clock();
            for (int i = 0; i < instance->n; i++) {
                instance->solution[i] = rand() % 2;
            }

            for (int i = 0; i < MAX_ITERATION; i++) {
                const unsigned long currentSum = calculateSum(instance);
                if (currentSum == instance->target) {
                    const clock_t end = clock();
                    result.execTime = ((double) (end - start)) / CLOCKS_PER_SEC;
                    result.solutionFound = 1;
                    break;
                }

                const int randIndex = rand() % instance->n;
                instance->solution[randIndex] = !instance->solution[randIndex];
            }

            if (result.solutionFound && result.execTime < bestResult.execTime) {
#pragma omp critical
                {
                    if (result.execTime < bestResult.execTime) {
                        bestResult = result;
                    }
                }
            }
        }
    }
    if (bestResult.execTime == DBL_MAX) {
        bestResult.execTime = -1;
    }
    return bestResult;
}