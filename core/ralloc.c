#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "./../utils/logger.h"

typedef struct {
  size_t chunks;
  size_t size;
  void *freeChunkMetaDataPtr;
  void *startPtr;
} Ralloc;

typedef struct {
  bool isFree;
  size_t size;
  struct ChunkMetaData *next;
} ChunkMetaData;

void dumpDebugChunkMetaData(ChunkMetaData *ptr) {
  LOG_INFO("Data from Chunk Meta Data: isFree '%d', size '%zu', cur '%p', next "
           "'%p', val '%p'",
           ptr->isFree, ptr->size, ptr, ptr->next, ptr + 1);
}

#define BASE_POOL_SIZE 1024 * 1024

Ralloc ralloc;
void dumpDebugRallocData() {
  LOG_INFO("Data from Ralloc Data: chunks '%zu', startPtr '%p', curPtr '%p'",
           ralloc.chunks, ralloc.startPtr, ralloc.freeChunkMetaDataPtr);
}

// For testing purpose only
void dumpRalloc(int chunks) {
  void *ptr = ralloc.startPtr;
  while (chunks > 0) {
    dumpDebugChunkMetaData(ptr);
    void *valuePtr = ptr + sizeof(ChunkMetaData);
    LOG_DEBUG("Value: %d", *(int *)valuePtr);
    ptr = ptr + sizeof(ChunkMetaData) + ((ChunkMetaData *)ptr)->size;
    chunks--;
  }
}

// For testing purpose only
int *dumpRallocToArr(int chunks) {
  int *arr = malloc(100000 * sizeof(int));
  int *preserve = arr;
  void *ptr = ralloc.startPtr;
  while (chunks > 0) {
    dumpDebugChunkMetaData(ptr);
    void *valuePtr = ptr + sizeof(ChunkMetaData);
    *arr = *(int *)valuePtr;
    ptr = ptr + sizeof(ChunkMetaData) + ((ChunkMetaData *)ptr)->size;
    arr++;
    chunks--;
  }
  return preserve;
}

void *initialize() {
  void *ptr = mmap(NULL, BASE_POOL_SIZE, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(ptr != MAP_FAILED && "Unable to Initialize Base Pool");
  LOG_INFO("Init for Base Pool Completed: %p\n", ptr);

  ralloc.chunks = 0;
  ralloc.size = BASE_POOL_SIZE;
  ralloc.startPtr = ptr;
  ralloc.freeChunkMetaDataPtr = ptr;

  return ptr;
}

void *rallocate(size_t size) {
  if (size == 0) {
    return NULL;
  }

  ChunkMetaData *curMetaDataPtr = (ChunkMetaData *)ralloc.freeChunkMetaDataPtr;
  while (curMetaDataPtr->next != NULL) {
    if (size <= curMetaDataPtr->size) {
      curMetaDataPtr->size = size;
      curMetaDataPtr->isFree = false;
      ralloc.chunks++;

      LOG_INFO("Allocating for chunk '%zu'", ralloc.chunks);
      dumpDebugChunkMetaData(curMetaDataPtr);

      void *retPtr = ralloc.freeChunkMetaDataPtr + sizeof(ChunkMetaData);
      ralloc.freeChunkMetaDataPtr = curMetaDataPtr->next;
      LOG_INFO("Returning Pointer from chunk: %p", retPtr);
      return retPtr;
    }

    curMetaDataPtr = (ChunkMetaData *)curMetaDataPtr->next;
  }

  void *retPtr = ralloc.freeChunkMetaDataPtr + sizeof(ChunkMetaData);
  void *nextPtr = retPtr + size;

  // TODO: can we optimize later ??
  size_t sizeDiff = (retPtr + size) - ralloc.startPtr;
  assert(sizeDiff <= ralloc.size && "Provided size exceeds Pool Size");

  curMetaDataPtr->size = size;
  curMetaDataPtr->next = NULL;
  curMetaDataPtr->isFree = false;
  ralloc.chunks++;

  LOG_INFO("Allocating for chunk 2'%zu'", ralloc.chunks);
  dumpDebugChunkMetaData(curMetaDataPtr);
  ralloc.freeChunkMetaDataPtr = nextPtr;

  LOG_INFO("Returning Pointer from chunk 2: %p", retPtr);
  return retPtr;
}

void ralloc_free(void *ptr) {
  assert(ptr != NULL && "Provide Not NULL pointer to free");

  *(int *)ptr = -1;
  ChunkMetaData *curMetaDataPtr =
      (ChunkMetaData *)(ptr - sizeof(ChunkMetaData));

  if (curMetaDataPtr->isFree == true) {
    return;
  }

  // TODO: disabling for testing purpose only
  /*assert(curMetaDataPtr->isFree == false && "Provide a occupied chunk");*/

  curMetaDataPtr->isFree = true;
  /*curMetaDataPtr->size = 0;*/
  curMetaDataPtr->next = ralloc.freeChunkMetaDataPtr;
  ralloc.freeChunkMetaDataPtr = curMetaDataPtr;
  ralloc.chunks--;
}
