/* bench.c is the benchmarking /test program for mem memory management
   bench.c is implemented as part of HW6, CSE374 22WI
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mem.h"

#define MAX_IN 50
void run_tests();
void storeHex(void *pointer, size_t size);
void freeHex(void *pointer);


/* Synopsis:   bench (main)
   [ntrials] (10000) getmem + freemem calls
   [pctget] (50) % of calls that are get mem
   [pctlarge] (10) % of calls requesting more memory than lower limit
   [small_limit] (200) largest size in bytes of small block
   [large_limit] (20000) largest size in byes of large block
   [random_seed] (time) initial seed for randn
*/
int main(int argc, char** argv ) {
  // bench[ntrials] [pctget] [pctlarge] [small_limit] [large_limit] [random_seed]
  int params[6] = {0, 0, 0, 0, 0, 0};

  // generate random seed
  time_t now;
  struct tm *tm;
  now = time(0);
  if ((tm = localtime(&now)) == NULL) {
    printf("Error extracting time stuff\n");
    return 1;
  }
  (argc > 1) ? (params[0] = atoi(argv[1])) : (params[0] = 10000); // ntrials
  (argc > 2) ? (params[1] = atoi(argv[2])) : (params[1] = 50); // pctget
  (argc > 3) ? (params[2] = atoi(argv[3])) : (params[2] = 10); // pctlarge
  (argc > 4) ? (params[3] = atoi(argv[4])) : (params[3] = 200); // small_limit
  (argc > 5) ? (params[4] = atoi(argv[5])) : (params[4] = 20000); // large_limit

  // initialize random number gen.
  (argc > 6) ? (params[5] = atoi(argv[6])) : (params[5] = tm->tm_hour * 60 + tm->tm_min); // random_seed

  run_tests(params);
   
  return EXIT_SUCCESS;
}

/* Define helper functions here. */

void run_tests(int* params) {
   int ntrials = params[0];
   int pctget = params[1];
   int pctlarge = params[2];
   int small_limit = params[3];
   int large_limit = params[4];
   int random_seed = params[5];

   void* blocks[ntrials];

   FILE *f = fopen("output.txt", "a");
   int size = 0;

   srand(random_seed);

   clock_t start_time, end_time;
   double cpu_timeElapse;
   start_time = clock();
   float percent = 0.1;

   for (int i = 0; i < ntrials; i++) {
    if ((rand() % 100 + 1) <= pctget) {  // rand num from 1 to 100
      void* p;
      // getmem
      if ((rand() % 100 + 1) <= pctlarge) {  // rand num from 1 to 100
        // Large block
        // rand num from small_limit - large_limit
        size_t pSize = (size_t) (rand() % (large_limit - small_limit)\
                                 + small_limit + 1);
        p = getmem(pSize);
        setFE(p, pSize);
      } else {
        // Small block
        // rand num from 1-small_limit
        size_t pSize = (size_t) (rand() % small_limit + 1);
        p = getmem(pSize);
        setFE(p, pSize);
      }
      blocks[size] = p;
      size++;
    } else {
      if (size > 0) {
        int index = rand() % size;
        freemem(blocks[index]);
        unsetFE(blocks[index]);
        blocks[index] = blocks[size - 1];
        size--;
      }
    }

    // pointers to store mem stats
    uintptr_t* total_size = (uintptr_t*) malloc(sizeof(uintptr_t*));
    uintptr_t* total_free = (uintptr_t*) malloc(sizeof(uintptr_t*));
    uintptr_t* n_free_blocks = (uintptr_t*) malloc(sizeof(uintptr_t*));
    
    if (((float) i / ntrials >= percent) || i == ntrials - 1) {
      end_time = clock();
      printf("end time is %ld\n", end_time);
      cpu_timeElapse = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
      get_mem_stats(total_size, total_free, n_free_blocks);
      printf("***--------------%d--------------***\n", \
             (int) (percent * 100));
      printf("CPU Time in seconds: %f\n", \
             cpu_timeElapse);
      printf("Total amount of storage from underlying system: %ld\n", \
             *total_size);
      printf("Total number of blocks in freelist: %ld\n", \
             *n_free_blocks);
      (n_free_blocks == 0) ? 
            printf("Average numbers of bytes in free storage: %d\n", \
            0):
            printf("Average numbers of bytes in free storage: %ld\n", \
            *total_free / *n_free_blocks);

      percent = percent + 0.1;
    }

    // Free pointers used for mem_stats
    free(total_size);
    free(total_free);
    free(n_free_blocks);
  }
  printf("\n");
  fclose(f);
}

// store hex value 0xFE to first 16 bytes
void storeHex(void *pointer, size_t size) {
  if (size > 16) {
    size = 16;
  }
  unsigned char filler = 0xFE;

  for (int i = 0; i < size; i++) {
    unsigned char *current = (unsigned char *) ((uintptr_t) pointer + i);
    *current = filler;
  }
}

// delete hex value and replace it with 0x00
void freeHex(void *pointer) {
  unsigned char filler = 0x00;
  for (int i = 0; i < 16; i++) {
    unsigned char *current = (unsigned char *) ((uintptr_t) pointer + i);
    *current = filler;
  }
}