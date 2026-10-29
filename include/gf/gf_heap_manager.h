#pragma once

// Local BrawlHeaders shadow: adds the heap table operations named from the linker map
// (init, registHeap, removeHeap, destroyHeap, calloc).

#include <StaticAssert.h>
#include <MEM/mem_allocator.h>
#include <memory.h>
#include <sr/sr_common.h>
#include <types.h>

class gfMemoryPool;

class gfHeapManager {
public:
    static void init();
    // HYPOTHESIS: heap ids are int as in createHeap; the pool type follows the stored name/start/end fields.
    static void registHeap(int heapId, gfMemoryPool* pool);
    static void removeHeap(int heapId);
    static void destroyHeap(int heapId);
    static void* calloc(Heaps::HeapType heapType, size_t size);

    static int getMaxFreeSize(Heaps::HeapType heapType);
    static void dumpAll();
    static void dumpList();
    static void createHeap(int heapId, const char* heapName, int memArena, int heapSize);

    static void* alloc(Heaps::HeapType heapType, size_t size);
    static void* alloc(Heaps::HeapType heapType, size_t size, s32 align);
    static void free(void* ptr);
    static MEMAllocator* getMEMAllocator(Heaps::HeapType heapType);
    static void* getHeap(Heaps::HeapType heapType);
};
