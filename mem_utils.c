/* mem_utils.c utilities to support memory management system.
   HW6, CSE 374
*/

#include "mem.h"
#include "mem_impl.h"

void check_heap() {
	if (freelist != NULL) {
		freeNode* current = freelist->next;
		while (current != NULL && current->next != NULL) {
	    // Memory aligned properly
	    // assert((uintptr_t)current % (uintptr_t)16 == 0);

        // Increasing memory addresses
        assert((uintptr_t)current < (uintptr_t)current->next);

        // Positive size and no smaller than min size
        assert(current->size > 0);
	    assert(current->size >= MINCHUNK);

        // Blocks Don't Overlap
        assert((uintptr_t)current + current->size + NODESIZE 
	           < (uintptr_t)current->next);

        // Blocks Don't Touch
        assert((uintptr_t)current + current->size + NODESIZE 
	           != (uintptr_t)current->next);
		}
    }
}

void get_mem_stats(uintptr_t* total_size, uintptr_t* total_free,
                   uintptr_t* n_free_blocks) {
  *total_size = totalmalloc;
  *total_free = 0;
  *n_free_blocks = 0;

  freeNode* currentNode = freelist;
  while (currentNode) {
    *n_free_blocks = *n_free_blocks + 1;
    *total_free = *total_free + (currentNode->size) + NODESIZE;
    currentNode = currentNode->next;
  }
}

void print_heap(FILE *f) {
  printf("Printing the heap\n");
  freeNode* currentNode = freelist;
  while (currentNode !=NULL) {
    fprintf(f, "%" PRIuPTR, (uintptr_t)currentNode);
    fprintf(f, ", size: %" PRIuPTR, currentNode->size);
    fprintf(f, ", next: %" PRIuPTR, (uintptr_t)currentNode->next);
    fprintf(f, "\n");
    currentNode = currentNode->next;
  }
}
