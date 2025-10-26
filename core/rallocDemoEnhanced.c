#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "./../utils/logger.h" // Assuming this works

// --- DATA STRUCTURES ---

// Use a union to enforce correct alignment, which is crucial for metadata.
// Also, ensure the size is at least as large as a pointer for the 'next' field,
// though a simple struct is often enough for basic allocators.
typedef struct ChunkMetaData {
  size_t size; // Size of the payload (user data) area
  bool isFree; // Status of the chunk
  struct ChunkMetaData
      *next; // Pointer to the next free chunk in the explicit free list
} ChunkMetaData;

typedef struct {
  size_t size;                 // Total size of the memory pool
  void *startPtr;              // Start of the memory pool
  ChunkMetaData *freeListHead; // Head of the explicit free list
} Ralloc;

#define BASE_POOL_SIZE (1024 * 1024)
#define MIN_SPLIT_SIZE                                                         \
  (2 * sizeof(ChunkMetaData)) // Minimum size for a remaining block to be
                              // considered split-worthy

Ralloc ralloc;

// --- UTILITY FUNCTIONS (Simplified) ---

void dumpDebugChunkMetaData(ChunkMetaData *ptr) {
  LOG_INFO("Chunk Meta Data: isFree '%d', size '%zu', cur '%p', next '%p', "
           "payload '%p'",
           ptr->isFree, ptr->size, ptr, ptr->next, (void *)(ptr + 1));
}

// --- INITIALIZATION ---

void *initialize() {
  // Check if already initialized
  if (ralloc.startPtr != NULL) {
    return ralloc.startPtr;
  }

  // mmap call (Corrected to use MAP_PRIVATE | MAP_ANONYMOUS)
  void *ptr = mmap(NULL, BASE_POOL_SIZE, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (ptr == MAP_FAILED) {
    perror("mmap failed");
    assert(false && "Unable to Initialize Base Pool");
    return NULL;
  }

  ralloc.size = BASE_POOL_SIZE;
  ralloc.startPtr = ptr;

  // CRITICAL FIX 1: Initialize the FIRST ChunkMetaData block
  ChunkMetaData *initial_meta = (ChunkMetaData *)ptr;
  initial_meta->isFree = true;

  // The initial free size is the total pool size minus the size of its own
  // metadata.
  initial_meta->size = BASE_POOL_SIZE - sizeof(ChunkMetaData);
  initial_meta->next = NULL;

  ralloc.freeListHead = initial_meta;

  LOG_INFO("Init for Base Pool Completed: %p\n", ptr);
  return ptr;
}

// --- ALLOCATION ---

static void split_block(ChunkMetaData *cur, size_t size_needed) {
  size_t total_size = cur->size + sizeof(ChunkMetaData);
  size_t new_meta_offset = sizeof(ChunkMetaData) + size_needed;

  // The remaining size must be large enough to hold a new ChunkMetaData
  // AND at least a small payload.
  if (total_size - new_meta_offset >= MIN_SPLIT_SIZE) {

    // 1. Calculate the start of the new free block's metadata
    ChunkMetaData *new_meta =
        (ChunkMetaData *)((char *)(cur + 1) + size_needed);

    // 2. Set up the new metadata for the remaining free block
    new_meta->isFree = true;
    // The new size is the old size minus the allocated space and the new
    // metadata
    new_meta->size = cur->size - size_needed - sizeof(ChunkMetaData);
    new_meta->next = cur->next; // Take over the 'next' pointer

    // 3. Update the current (allocated) block's size and link
    cur->size = size_needed;
    cur->next = new_meta; // Temporarily link for easy free list insertion
  }
}

void *rallocate(size_t size) {
  if (size == 0) {
    return NULL;
  }

  // 1. Search the Free List (First-Fit Strategy)
  ChunkMetaData *current = ralloc.freeListHead;
  ChunkMetaData *prev = NULL;

  while (current != NULL) {
    if (current->isFree && current->size >= size) {

      // 2. Found a block! Now check if we can split it.
      split_block(current, size);

      // 3. Update the Free List pointers (Remove the allocated block)
      if (prev == NULL) {
        // We allocated the head block; move the head to the remaining part (if
        // split)
        ralloc.freeListHead = current->next;
      } else {
        // Link the previous block to the next free block
        prev->next = current->next;
      }

      // 4. Mark the block as allocated
      current->isFree = false;
      current->next = NULL; // Clear the free list pointer

      LOG_INFO("Allocated chunk size '%zu' at %p", current->size, current);

      // CRITICAL FIX 2: Return pointer to the payload (metadata + 1)
      return (void *)(current + 1);
    }

    prev = current;
    current = current->next;
  }

  // 5. Out of memory (No simple bump allocation at the end is allowed with a
  // free list!)
  LOG_ERROR("Out of memory: No suitable free chunk found.");
  return NULL;
}

// --- FREEING ---

void ralloc_free(void *ptr) {
  assert(ptr != NULL && "Provide Not NULL pointer to free");

  // CRITICAL FIX 3: Get pointer to metadata by subtracting its size
  ChunkMetaData *curMetaDataPtr = (ChunkMetaData *)ptr - 1;

  if (curMetaDataPtr->isFree == true) {
    LOG_WARN("Double free attempt detected for pointer: %p", ptr);
    return;
  }

  curMetaDataPtr->isFree = true;

  // 1. Insert the freed block at the head of the free list (LIFO)
  curMetaDataPtr->next = ralloc.freeListHead;
  ralloc.freeListHead = curMetaDataPtr;

  LOG_INFO("Freed pointer '%p', metadata at %p", ptr, curMetaDataPtr);

  // 2. Coalescing (Merging adjacent free blocks)
  // NOTE: This simple LIFO free requires an extra pass to search for
  // *physically* adjacent free blocks. The easiest coalescing is to check if
  // the new block is adjacent to the block currently at the head.

  // Start with the newly freed block (now the head of the list)
  ChunkMetaData *current = ralloc.freeListHead;
  ChunkMetaData *prev = NULL;

  while (current != NULL) {

    // Check forward coalescing: Is the next block in memory also free?
    // Check if the current chunk's end meets the next chunk's metadata start
    ChunkMetaData *next_in_memory =
        (ChunkMetaData *)((char *)(current + 1) + current->size);

    if (current->next != NULL && current->next == next_in_memory) {
      // Merge the current chunk with its physical neighbor (which is also the
      // logical next free chunk)
      current->size += (current->next->size + sizeof(ChunkMetaData));
      current->next =
          current->next->next; // Bypass the merged chunk in the free list

      // Coalesced successfully, continue checking from the current block
      continue;
    }

    // Check backward coalescing: Is the previous block in memory also free?
    // This requires iterating the entire list to find the memory-predecessor,
    // which is complex and slow. For simplicity, we only do forward checks in
    // this basic version.

    prev = current;
    current = current->next;
  }
}
