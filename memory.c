/* memory.c
   implements getmem (malloc) and freemem (free) functions for memory system
   CSE 374 HW6
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

/* This code will be filled in by the students.  Your primary job is to
   implement getmem and freemem functions.  */

/* initialize global variables */

/* What helper functions will you need?  Declare here. */

/* Define your functions below: */

void* getmem(uintptr_t size) {
  /* make sure you return a pointer to the usable memory that
     is at least 'size' bytes long.
     To get you started we are 'stubbing in' a call that will
     return a usable value.  You will replace this code. */

  return malloc(size);
}

