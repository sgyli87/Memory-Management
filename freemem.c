/* freemem.c
   implements freemem
   CSE 374 HW6
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

// helper function declarations
void insertFreeNode(freeNode* my_freeList,
                    size_t free_size,
                    uintptr_t free_addr);
int checkAdjacency(uintptr_t addr1, uintptr_t addr2, size_t size1, size_t size2);
void groupAdjacent(freeNode* my_freeList);

// Free the block of storage pointed by pointer p.
void freemem(void* p) {
  if (p == NULL) {
    return;
  }
  uintptr_t addr = (uintptr_t) p;
  size_t *sizePtr = (size_t *) (addr - OFFSET);
  size_t size = *sizePtr;

  insertFreeNode(freeList, size, addr);
  check_heap();
}

/* 
  Inserts free node by user in given position
  while preserving ascending addresses
*/
void insertFreeNode(freeNode* my_freeList,
                    size_t free_size,
                    uintptr_t free_addr) {
  freeNode* curr = my_freeList;
  int added_node = 0;

  // empty case
  if (curr == NULL) {
    insertFirstNode(free_size, free_addr, NULL);
    return;
  }

  // add node in front of the head
  if (free_addr < curr->addr) {
    insertFirstNode(free_size, free_addr, curr);
    groupAdjacent(freeList);
    groupAdjacent(freeList);
    return;
  }

  // node in the middle
  while (curr->next != NULL) {
    // add node in specified postion and keep ascending address order
    if (curr->next->addr > free_addr) {
      freeNode* newNode = makeNewNode(free_size, free_addr, curr->next);
      curr->next = newNode;
      added_node = 1;
      break;
    }
    curr = curr->next;
  }

  // add node into the end
  if (!added_node) {
    curr->next = makeNewNode(free_size, free_addr, NULL);
  }

  // clean up the list by grouping adjacent nodes
  groupAdjacent(my_freeList);
  groupAdjacent(my_freeList);
}

// check if the two nodes are next to each other
int checkAdjacency(uintptr_t addr1, uintptr_t addr2, size_t size1, size_t size2) {
  if (addr1 > addr2) {
    return (addr1 - addr2 - size2) == OFFSET;
  } else {
    return (addr2 - addr1 - size1) == OFFSET;
  }
}

/* 
  scan through the list and combine nodes from
  if they are adjacent.
*/
void groupAdjacent(freeNode* my_freeList) {
  freeNode* curr = my_freeList;

  if (curr == NULL) {
    fprintf(stderr, "Error: empty list");
  }

  while (curr->next != NULL) {
    if (isAdjacent(curr->addr, \
                   curr->next->addr, \
                   curr->size, \
                   curr->next->size)) {
      // group nodes
      // address doesn't need to be changed since current will always be less)
      freeNode* nextNode = curr->next;
      curr->size = curr->size + nextNode->size + OFFSET;
      curr->next = nextNode->next;
      free(nextNode);
    } else {
    curr = curr->next;
    }
  }
}