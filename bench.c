/* bench.c is the benchmarking /test program for mem memory management
   bench.c is implemented as part of HW6, CSE374 22WI
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mem.h"

#define MAX_IN 50
int setParamInfo(char* param_name, int* params);
int strToInt(char* string);
void printParams(int* params, int size);
void perform_bench_tests();
void setFE(void *pointer, size_t size);
void unsetFE(void *pointer);


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
  int params[6] = {10000, 50, 10, 200, 20000, 0};

  // generate random seed
  time_t t;
  struct tm *tm;
  t = time(0);

  if((tm == localtime(&t))== NULL){
   printf("Failed to extract time.\n");
   return 1;
  }
  params[5] = tm->tm_hour * 60 + tm->tm_min;

  for(int i = 0; i < argc; i++){
   setParamInfo(argv[i],params);
  }
  
  perform_bench_tests(params);
   
  return EXIT_SUCCESS;
}

/* Define helper functions here. */

void perform_bench_tests(int* params) {
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
      printf("end is %ld\n", end_time);
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

void printParams(int* params, int size) {
  for (int i = 0; i < size; i++) {
    printf("%d ", params[i]);
  }
  printf("\n");
}

int setParamInfo(char* param_string, int* params) {
  // Split string
  int i = 0;
  int e_index = -1;
  // just use strchr here
  while (param_string[i]) {
    if (param_string[i] == '=') {
      e_index = i;
    }
    i++;
  }

  if (e_index == -1 || e_index == strlen(param_string) - 1) {
    fprintf(stderr, "Error: Bad param '%s'\n", param_string);
    exit(EXIT_FAILURE);
  }

  char param_name[MAX_IN];
  char param_strNum[MAX_IN];

  strncpy(param_name, param_string, e_index);
  strncpy(param_strNum, param_string + e_index + 1, strlen(param_name));

  int param_num = strToInt(param_strNum);

  // Set parameters
  // ntrials, pctget, pctlarge, small_limit, large_limit, random_seed
  if (!strncmp(param_name, "ntrials", e_index)) {
    params[0] = param_num;
  } else if (!strncmp(param_name, "pctget", e_index)) {
    params[1] = param_num;
  } else if (!strncmp(param_name, "pctlarge", e_index)) {
    params[2] = param_num;
  } else if (!strncmp(param_name, "small_limit", e_index)) {
    params[3] = param_num;
  } else if (!strncmp(param_name, "large_limit", e_index)) {
    params[4] = param_num;
  } else if (!strncmp(param_name, "random_seed", e_index)) {
    params[5] = param_num;
  } else {
    fprintf(stderr, "Error: Bad param name %s\n", param_name);
    exit(EXIT_FAILURE);
  }
  return 0;
}

int strToInt(char* str) {
  int result = 0;
  int i = 0;
  while (str[i]) {
    if (str[i] >= '0' && str[i] <= '9') {
      result = (result * 10) + (str[i]-'0');
      str++;
    } else {
      fprintf(stderr, "Error: Invalid string to int '%s'\n", str);
      exit(EXIT_FAILURE);
    }
  }
  return result;
}

void setFE(void *pointer, size_t size) {
  if (size > 16) {
    size = 16;
  }
  unsigned char filler = 0xFE;

  for (int i = 0; i < size; i++) {
    unsigned char *current = (unsigned char *) ((uintptr_t) pointer + i);
    *current = filler;
  }
}

void unsetFE(void *pointer) {
  unsigned char filler = 0x00;
  for (int i = 0; i < 16; i++) {
    unsigned char *current = (unsigned char *) ((uintptr_t) pointer + i);
    *current = filler;
  }
}