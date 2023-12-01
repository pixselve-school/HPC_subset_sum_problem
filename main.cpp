
/*
 * Subset-Sum Problem (SSP)
 *
 * ESIR HPC project 2023
 *
 * last update: November 18th, 2023
 *
 * AM
 */

#include <climits>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

// SSP data structure
// - "n" is the size of the instance (# of elements in the set)
// - "solution" is an array of unsigned int that contains one of the known solutions (binary)
// - "target" is the (unsigned long) integer target
// - "set" is the pointer to the array of (unsigned long) integer values
struct ssp
{
   unsigned int n;
   unsigned int* solution;
   unsigned long target;
   unsigned long* set;
};
typedef struct ssp SSP;

// SSP printer
void printSSP(SSP instance)
{
   unsigned int i;
   printf("Subset-Sum instance (n = %u, target = %lu)\n",instance.n,instance.target);
   printf("Set (solution in binary) = {");
   for (i = 0; i < instance.n; i++)
   {
      printf("%lu (%1d)",instance.set[i],instance.solution[i]);
      if (i != instance.n - 1)  printf(", ");
   };
   printf("}\n");
}

// log2 with bit shifts
unsigned int ln(unsigned long number)
{
   unsigned int log = 0UL;
   while (number > 0)
   {
      number = number >> 1; // correction of the bit shift
      log++;
   };
   return log;
}

// SSP instance generator
// - "n" is the size of the instance (input, >0)
// - "instance" is a pointer to an 'empty' variable of type SSP (input and output)
// - "max" is the maximum allowed value for the integers in the set (>1, input)
// The returning value is the 'density' of the instance
double genSSP(int n,SSP *instance,unsigned long max)
{
   int i;
   unsigned int nbits;
   unsigned long element;
   unsigned long current_max = 0;
   double density = 0.0;

   // initializing the instance (we suppose that no memory is associated to inner pointers)
   instance->n = n;
   instance->solution = (unsigned int*)calloc(n,sizeof(unsigned int));
   instance->target = 0L;
   instance->set = (unsigned long*)calloc(n,sizeof(unsigned long));

   // seed of random number generator
   srand(6666);

   // creating the instance with one of its solutions
   for (i = 0; i < n; i++)
   {
      element= 1UL + rand()%(max - 1);
      if (current_max < element)
      {
         current_max = element;
         nbits = ln(element);
         density = (double) n/nbits;
      }
      instance->solution[i] = 0;
      if (rand()%2)
      {
         instance->target = instance->target + element;
         instance->solution[i] = 1;
      }
      instance->set[i] = element;
   };

   return density;
}

// freeSSP
void freeSSP(SSP *instance)
{
   free(instance->solution);
   free(instance->set);
}

// Function to calculate the sum of a subset
unsigned long subsetSum(SSP *instance, unsigned int* subsetIndices, unsigned int subsetSize) {
    unsigned long sum = 0;
    for (unsigned int i = 0; i < subsetSize; i++) {
        sum += instance->set[subsetIndices[i]];
    }
    return sum;
}

// Hill climbing algorithm for SSP
bool hillClimbingSSP(SSP *instance) {
    unsigned int subsetIndices[instance->n];
    unsigned int subsetSize = 0;
    unsigned long currentSum = 0;
    unsigned long bestSum = labs(instance->target - currentSum);

    while (true) {
        int bestCandidate = -1;
        unsigned long bestCandidateSum = bestSum;

        // Generate neighbors and find the best
        for (unsigned int i = 0; i < instance->n; i++) {
            // Try adding an element
            subsetIndices[subsetSize] = i;
            unsigned long newSum = labs(instance->target - subsetSum(instance, subsetIndices, subsetSize + 1));
            if (newSum < bestCandidateSum) {
                bestCandidate = i;
                bestCandidateSum = newSum;
            }
            // Reset the change
            subsetIndices[subsetSize] = 0;
        }

        // Check if there's no improvement
        if (bestCandidateSum >= bestSum) {
            break;
        }

        // Update the current subset
        subsetIndices[subsetSize++] = bestCandidate;
        bestSum = bestCandidateSum;

        // Check if the target is achieved
        if (bestSum == 0) {
            for (unsigned int i = 0; i < subsetSize; i++) {
                instance->solution[subsetIndices[i]] = 1;
            }
            return true;
        }
    }

    return false;
}




// Hill climbing algorithm for SSP with OpenMP
bool hillClimbingSSPOpenMP(SSP *instance) {
    bool found = false;
    unsigned long bestSum = ULONG_MAX;
    unsigned int bestSubset[instance->n];
    unsigned int bestSubsetSize = 0;

#pragma omp parallel
    {
        unsigned int localSubset[instance->n];
        unsigned int localSubsetSize = 0;
        unsigned long localBestSum = ULONG_MAX;

#pragma omp for nowait
        for (unsigned int i = 0; i < instance->n; i++) {
            localSubset[localSubsetSize++] = i;
            unsigned long localSum = subsetSum(instance, localSubset, localSubsetSize);

            if (labs(instance->target - localSum) < localBestSum) {
                localBestSum = labs(instance->target - localSum);
                memcpy(localSubset, localSubset, localSubsetSize * sizeof(unsigned int));
            }
        }

#pragma omp critical
        {
            if (localBestSum < bestSum) {
                bestSum = localBestSum;
                memcpy(bestSubset, localSubset, localSubsetSize * sizeof(unsigned int));
                bestSubsetSize = localSubsetSize;
            }
        }
    }

    if (bestSum == 0 || bestSum < ULONG_MAX) {
        for (unsigned int i = 0; i < bestSubsetSize; i++) {
            instance->solution[bestSubset[i]] = 1;
        }
        found = true;
    }

    return found;
}



// Benchmarking function
void benchmarkSSP(SSP *instance, bool (*sspFunc)(SSP *)) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    bool found = sspFunc(instance);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Execution Time: %f seconds\n", cpu_time_used);
    if (found) {
        printf("Solution found:\n");
        // printSSP(*instance);
    } else {
        printf("No solution found\n");
    }
}

// main
int main() {
    int n = 10000;
    SSP instance;
    double density = genSSP(n, &instance, 100);
    // printSSP(instance);
    printf("Density is %lf\n", density);

    printf("\nBenchmarking Sequential Hill Climbing:\n");
    benchmarkSSP(&instance, hillClimbingSSP);

    // Resetting solution array for next benchmark
    memset(instance.solution, 0, instance.n * sizeof(unsigned int));

    printf("\nBenchmarking Parallel Hill Climbing with OpenMP:\n");
    benchmarkSSP(&instance, hillClimbingSSPOpenMP);

    freeSSP(&instance);
    return 0;
}

