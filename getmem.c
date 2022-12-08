/*
  Copyright[2022] <Li> Legal/Copyright
  Author: Yang Li, Rico Li
  UWNetID: yangli99, kangll
  Homework 6
  Date: 12/8/2022
  Function: getmem.c returns a pointer to a 
			new block of storage with at least 
			size bytes of memory. The pointer 
			to the returned block is aligned 
			on an 16-byte boundary.
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

#define EXTRA_SIZE 4096

/* initialize global variables */
freeNode* freelist = NULL;
uintptr_t totalmalloc = 0;

/* helper functions*/
size_t spaceAvail(freeNode *freeList, uintptr_t size);
void* split(freeNode* freeList, uintptr_t size, size_t space);
void extendSpace(uintptr_t size);
void freeLastNode(size_t size);

/* Define your functions below: */

void* getmem(uintptr_t size) {
   check_heap();
   if (size <= 0) {
      return NULL;
   }

   size += size % 16 == 0 ? 0 : 16 - size % 16;

   size_t space = spaceAvail(freelist, size);

   if (space == 0) {
      extendSpace(size);
      if (freelist == NULL) {
         fprintf(stderr, "No memory available.\n");
         return NULL;
      }
      space = size;
   }

   check_heap();
   return split(freelist, size, space);
   check_heap();
}

/*
 checks if the freelist has available block whose size is bigger than or equal to the given
 size. If not return 0, otherwise return the size.
*/ 
size_t spaceAvail(freeNode* curr, uintptr_t size){
   while (curr != NULL) {
      if (curr->size >= size) {
         return curr->size;
      }
      curr = curr->next;
   }
   return 0;
}

/*
  The function identifies the block with size given in the freelist,
  and it will update the splited block with new parameters.
  If approriate size is found, splits and adds a new block and returns
  a pointer to the newly freed block
*/
void* split(freeNode* freeList1, uintptr_t size,size_t space){
   freeNode* curr = freeList1;

   if (freeList1->size == space) {
      size_t *firstPart = (size_t *) (curr->addr - OFFSET);
      *firstPart = (size_t) size;
      uintptr_t firstPartHead = curr->addr;
      uintptr_t firstPartBack = firstPartHead + size;
      
      size_t newFirstSize = curr->size - size - OFFSET;

      if((intptr_t) newFirstSize > OFFSET*2){
         size_t *remainFirstPart = (size_t *)firstPartBack;
         *remainFirstPart = newFirstSize;
         uintptr_t newFirstAddr = firstPartBack + OFFSET;
         curr->size = newFirstSize;
         curr->addr = newFirstAddr;
      }else{
         *firstPart = *firstPart + newFirstSize + OFFSET;
         freelist = freeList1->next;
         free(curr);
      }
      return (void *) firstPartHead;
   }

   // travese to one of the block that will have space 
   while (curr->next != NULL && curr->next->size != space) {
      curr = curr->next;
   }

   // if only one block presents in the list, either at the 
   // beginning or the last

   if (curr->next == NULL){
      size_t *lastPart = (size_t *)(curr->addr - OFFSET);
      *lastPart = (size_t)size;
      uintptr_t lastPartHead = curr->addr;
      uintptr_t lastPartEnd = lastPartHead + size;
      size_t newLastSize = curr->size - size - OFFSET;

      if((intptr_t)newLastSize > OFFSET*4) {
         size_t *remainLastPart = (size_t *)lastPartEnd;
         *remainLastPart = newLastSize;
         uintptr_t newLastAddr = lastPartEnd + OFFSET;
         curr->size = newLastSize;
         curr->addr = newLastAddr;
      }else{
         *lastPart = *lastPart + newLastSize + OFFSET;
         freeLastNode(size);
      }
      return (void *) lastPartHead;
   }else{
      size_t *midPart = (size_t *)(curr->next->addr - OFFSET);
      *midPart = (size_t) size;
      uintptr_t midPartHead = curr->next->addr;
      uintptr_t midPartEnd = midPartHead + size;

      freeNode* rest = curr->next->next;
      size_t newSize = curr->next->size - size - OFFSET;

      if ((intptr_t) newSize > OFFSET*4) {
         size_t *remainPart = (size_t *)midPartEnd;
         *remainPart = newSize;
         uintptr_t newAddr = midPartEnd + OFFSET;

         curr->next->size = newSize;
         curr->next->addr = newAddr;
      }else{
         *midPart = *midPart + newSize + OFFSET;
         free(curr->next);
         curr->next = rest;
      }
      return (void *) midPartHead;
   }
}

// creates a new node and returns it
freeNode* makeNewNode(size_t size, uintptr_t addr, freeNode* next) {
  freeNode* newNode = (freeNode *) malloc(NODESIZE);
  if (newNode == NULL) {
    printf("newNode is null\n");
    return NULL;
  }
  newNode->size = size;
  newNode->addr = addr;
  newNode->next = next;
  return newNode;
}

/* 
   insert a new node at the beginning.
*/
void insertFirstNode(size_t f_size, uintptr_t f_addr, freeNode* next) {
  freelist = makeNewNode(f_size, f_addr, next);
}

/*
   insert a new node while keep the ascending address order
*/
void insertNewNode(freeNode* list, size_t newSize, uintptr_t newAddr){
   if (freelist == NULL) {
      freelist = makeNewNode(newSize, newAddr, NULL);
   }

   if(newAddr < freelist->addr){
      freeNode* newNode = makeNewNode(newSize, newAddr, list);
      freelist = newNode;
   } else {
      freeNode* curr = list;
      while (curr->next != NULL && newAddr > curr->next->addr) {
         curr = curr->next;
      }
      curr->next = makeNewNode(newSize, newAddr, curr->next);
   }
}

/* 
   request memory space of predefined size from underlying 
   system will be called when no available space in freelist
*/
void extendSpace(uintptr_t size){
   uintptr_t requestSize = size > EXTRA_SIZE ? 
                           size + (size % 16) : EXTRA_SIZE;

   uintptr_t newMem = (uintptr_t)malloc(requestSize + OFFSET);
   if((void *) newMem == NULL){
      fprintf(stderr, "Error: Failed to assign.\n");
      freelist = NULL;
   }
   totalmalloc += requestSize + OFFSET;

   size_t newSize = requestSize;
   size_t *newSizePtr = (size_t *)newMem;
   *newSizePtr = newSize;
   uintptr_t newAddr = newMem + OFFSET;

   if (freelist == NULL) {
      freelist = makeNewNode(newSize, newAddr, NULL);
   } else {
      insertNewNode(freelist, newSize, newAddr);
   }
}

// remove the last node in the freeList given its size.
void freeLastNode(size_t size) {
  freeNode* curr = freelist;
  while (curr != NULL && curr->next != NULL) {
    if (curr->next->size == size) {
      free(curr->next);
      curr->next = NULL;
    }
    curr = curr->next;
  }
}