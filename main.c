#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>

#include "core/ralloc.c"

void dumpRallocPool() {
  for (int i = 0; i < 5; i++) {
    int *ptr = (int *)rallocate(sizeof(int));
    ralloc_free(ptr);
  }
}

/*void dumpRallocFreeList() {*/
/*  RallocFreeListNode *ptr = rallocFreeList.headPtr;*/
/*  LOG_DEBUG("Head Ptr: %p", ptr);*/
/*  LOG_DEBUG("Free List Chunk: %p", ptr->chunkPtr);*/
/*  while (ptr != NULL) {*/
/*    LOG_DEBUG("Free List Chunk: %p", ptr->chunkPtr);*/
/*    ptr = (RallocFreeListNode *)ptr->nextPtr;*/
/*  }*/
/*}*/

int main() {
  initialize();
  dumpRallocPool();
  return 0;
}
