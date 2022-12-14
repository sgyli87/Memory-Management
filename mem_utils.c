/*
  Copyright[2022] <Li> Legal/Copyright
  Author: Yang Li, Rico Li
  UWNetID: yangli99, kangll
  Homework 6
  Date: 12/8/2022
*/

#include "mem.h"
#include "mem_impl.h"

void check_heap() {
    freeNode* current = freelist;

    while (current != NULL && current->next != NULL) {
    // Increasing memory addresses
    assert(current->addr < current->next->addr);

    // Positive size and greater than min size
    assert(current->size > 0);
	assert(current->size >= MINCHUNK);

    // Blocks Don't Overlap
    assert((current->addr + current->size) + OFFSET 
	       < (current->next->addr));

    // Blocks Don't Touch
    assert((current->addr + current->size) + OFFSET 
	       != (current->next->addr));

    current = current->next;
    }
    return;
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
