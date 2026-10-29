#pragma once

// Local BrawlHeaders shadow: verified pool and block layouts and the pool operations named
// from the linker map (create, clear, destory, calloc, createMEMAllocator).

#include <StaticAssert.h>
#include <revolution/MEM/mem_allocator.h>
#include <revolution/OS/OSMutex.h>
#include <types.h>

class gfMemoryPool;

struct HeapInfo {
    const char* m_name;
    gfMemoryPool* m_memoryPool;
    u32 m_size;
    u32 m_arena;
};
extern HeapInfo g_HeapInfos[0x47];

// HYPOTHESIS: local name. Every block of a pool, free or allocated, starts with this header;
// the free blocks form a circular list sorted by address.
struct gfMemoryPoolBlock {
    gfMemoryPool* m_pool;
    u32 m_size;
    gfMemoryPoolBlock* m_next;
    const char* m_file;
    u16 m_line;
    u8 m_id;
    u8 m_flags;
    u8 unk14[0xC];
};
static_assert(sizeof(gfMemoryPoolBlock) == 0x20, "Class is wrong size!");

// HYPOTHESIS: local name. The words just below every allocated address.
struct gfMemoryPoolTag {
    gfMemoryPoolBlock* m_block;
    u32 unk4;
    u32 m_magic;
};

class gfMemoryPool {
public:
    const char* m_name;
    void* m_start;
    void* m_end;
    u32 m_size;
    u8 m_id;
    u8 m_isSubPool : 1;
    u8 m_debugFlags;
    u16 m_allocCount;
    u16 m_peakAllocCount;
    u32 m_peakUsage;
    u32 m_usage;
    gfMemoryPoolBlock* m_blocks;
    bool m_useMutex;
    OSMutex m_mutex;
    MEMAllocator m_allocator;

    // HYPOTHESIS: parameter types follow create(void*, size_t, const char*).
    gfMemoryPool(const char* heapName, void* heapAddr, size_t size);
    void clear();
    u32 getMaxFreeBlockSize();
    // HYPOTHESIS: both parameters are stored as the allocator's heap parameters.
    void createMEMAllocator(int heapId, int param);

    static void* alloc(void* heapAddr, size_t size, int align);
    // HYPOTHESIS: same parameter list as alloc.
    static void* calloc(void* heapAddr, size_t size, int align);
    static gfMemoryPool* create(void* heapAddr, size_t size, const char* heapName);
    // HYPOTHESIS: a sub-pool allocated from another pool.
    static gfMemoryPool* create(void* heapAddr, size_t size);
    static void destory(gfMemoryPool* pool);

    /**
     * @brief Custom function to create a memory pool at a specific address
     *
     * @param heapAddr Start address of the memory pool
     * @param heapName Name to assign the memory pool
     * @param heapID ID of the memory pool
     * @param size Size of the memory pool
     * @param arena Whether this pool resides in MEM1 or MEM2
     */
    static gfMemoryPool* create(void* heapAddr, const char* heapName, int heapID, size_t size, int arena)
    {
        gfMemoryPool* pool = create(heapAddr, size, heapName);
        g_HeapInfos[heapID].m_name = heapName;
        g_HeapInfos[heapID].m_memoryPool = pool;
        g_HeapInfos[heapID].m_size = size;
        g_HeapInfos[heapID].m_arena = arena;
        return pool;
    }
}; // class gfMemoryPool
static_assert(sizeof(gfMemoryPool) == 0x50, "Class is wrong size!");
