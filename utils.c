#include <stdio.h>
#include <stdlib.h>

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
   // srand();

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


// ================================================================================

typedef struct {
   double execTime;
   int solutionFound;  // 1 if found, 0 otherwise
} HillClimbingResult;