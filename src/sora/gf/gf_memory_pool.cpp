#include <gf/gf_memory_pool.h>
#include <new>
#include <revolution/BASE/PPCArch.h>
#include <revolution/OS/OSError.h>
#include <revolution/OS/OSMutex.h>
#include <string.h>
#include <types.h>

extern "C" void fn_80025F2C(gfMemoryPool* pool, void* ptr);
extern "C" void fn_800260B4(gfMemoryPool* pool);

// HYPOTHESIS: local name; a scoped lock that only locks pools with m_useMutex set.
struct gfMemoryPoolLock {
    OSMutex* m_mutex;
    bool m_locked;

    gfMemoryPoolLock(gfMemoryPool* pool) : m_mutex(&pool->m_mutex), m_locked(pool->m_useMutex) {
        if (m_locked) {
            OSLockMutex(m_mutex);
        }
    }
    ~gfMemoryPoolLock() {
        if (m_locked) {
            OSUnlockMutex(m_mutex);
        }
    }
};

gfMemoryPool* gfMemoryPool::create(void* heapAddr, size_t size, const char* heapName) {
    return new (heapAddr) gfMemoryPool(heapName, heapAddr, size);
}

gfMemoryPool* gfMemoryPool::create(void* heapAddr, size_t size) {
    gfMemoryPool* pool = create(alloc(heapAddr, size, 0x20), size, NULL);
    if (pool != NULL) {
        pool->m_isSubPool = true;
    }
    return pool;
}

void gfMemoryPool::clear() {
    gfMemoryPoolBlock* head = reinterpret_cast<gfMemoryPoolBlock*>((reinterpret_cast<u32>(this) + sizeof(gfMemoryPool) + 0x1F) & ~0x1F);
    gfMemoryPoolBlock* body = head + 1;
    u32 bodySize = static_cast<u8*>(m_end) - reinterpret_cast<u8*>(body);
    m_blocks = head;
    head->m_pool = this;
    head->m_size = 0;
    head->m_next = body;
    head->m_file = NULL;
    head->m_line = 0;
    head->m_id = 0;
    head->m_flags = 0;
    body->m_pool = this;
    body->m_size = bodySize;
    body->m_next = head;
    body->m_file = NULL;
    body->m_line = 0;
    body->m_id = 0;
    body->m_flags = 0;
}

// HYPOTHESIS: the checked free shared by destory and gfPoolFree.
static inline void freeChecked(void* ptr) {
    if (ptr != NULL) {
        gfMemoryPoolTag* tag = static_cast<gfMemoryPoolTag*>(ptr) - 1;
        if (tag->m_magic != 0xCCCCCCCC) {
            OSReport("gfPoolFree Error:\n");
            if (tag->m_magic == 0xDDDDDDDD) {
                gfMemoryPoolBlock* block = tag->m_block;
                OSReport("<<This memory block Already free>>\n");
                OSReport("address: 0x%08x, %s(%d)\n", ptr, block->m_file, block->m_line);
            } else {
                u32* words = reinterpret_cast<u32*>(reinterpret_cast<u32>(ptr) & ~0xF);
                OSReport("address: 0x%08x\n", ptr);
                for (int i = 0; i < 2; i++) {
                    u32* line = words + i * 4;
                    OSReport("0x%08x: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n", line, line[0], line[1], line[2], line[3]);
                }
            }
        }
        fn_80025F2C(tag->m_block->m_pool, ptr);
    }
}

void gfMemoryPool::destory(gfMemoryPool* pool) {
    if (pool != NULL && pool->m_isSubPool) {
        freeChecked(pool);
    }
}

gfMemoryPool::gfMemoryPool(const char* heapName, void* heapAddr, size_t size) {
    m_name = heapName;
    m_id = 0;
    m_isSubPool = false;
    m_debugFlags = 0;
    m_allocCount = 0;
    m_peakAllocCount = 0;
    m_peakUsage = 0;
    m_usage = 0;
    m_useMutex = false;
    OSInitMutex(&m_mutex);
    m_start = heapAddr;
    u8* end = static_cast<u8*>(heapAddr) + size;
    m_end = end;
    m_size = size;
    gfMemoryPoolBlock* head = reinterpret_cast<gfMemoryPoolBlock*>((reinterpret_cast<u32>(this) + sizeof(gfMemoryPool) + 0x1F) & ~0x1F);
    m_blocks = head;
    gfMemoryPoolBlock* body = head + 1;
    head->m_pool = this;
    head->m_size = 0;
    head->m_next = body;
    head->m_file = NULL;
    head->m_line = 0;
    head->m_id = 0;
    head->m_flags = 0;
    body->m_pool = this;
    body->m_size = end - reinterpret_cast<u8*>(body);
    body->m_next = head;
    body->m_file = NULL;
    body->m_line = 0;
    body->m_id = 0;
    body->m_flags = 0;
}

void* gfMemoryPool::alloc(void* heapAddr, size_t size, int align) {
    gfMemoryPool* pool = static_cast<gfMemoryPool*>(heapAddr);
    gfMemoryPoolLock lock(pool);
    if (size != 0) {
        if (align < 4) {
            align = 4;
        }
        gfMemoryPoolBlock* prev = pool->m_blocks;
        u32 mask = ~(align - 1);
        u8 id = pool->m_id;
        gfMemoryPoolBlock* block = prev->m_next;
        for (;;) {
            if (block->m_size >= size + sizeof(gfMemoryPoolBlock)) {
                u8* end = reinterpret_cast<u8*>(block) + block->m_size;
                u8* ptr = reinterpret_cast<u8*>(reinterpret_cast<u32>(end - size) & mask);
                gfMemoryPoolBlock* used = reinterpret_cast<gfMemoryPoolBlock*>(reinterpret_cast<u32>(ptr - sizeof(gfMemoryPoolBlock)) & mask);
                if (used >= block) {
                    if (used == block) {
                        prev->m_next = block->m_next;
                        used = block;
                    } else {
                        used->m_pool = pool;
                        used->m_size = end - reinterpret_cast<u8*>(used);
                        used->m_next = NULL;
                        used->m_file = NULL;
                        used->m_line = 0;
                        used->m_id = 0;
                        used->m_flags = 0;
                        block->m_size = reinterpret_cast<u8*>(used) - reinterpret_cast<u8*>(block);
                    }
                    gfMemoryPoolTag* tag = reinterpret_cast<gfMemoryPoolTag*>(ptr) - 1;
                    tag->m_block = used;
                    tag->m_magic = 0xCCCCCCCC;
                    used->m_id = id;
                    used->m_flags &= ~1;
                    pool->m_allocCount++;
                    pool->m_usage += used->m_size;
                    pool->m_peakUsage = pool->m_peakUsage < pool->m_usage ? pool->m_usage : pool->m_peakUsage;
                    pool->m_peakAllocCount = pool->m_peakAllocCount < pool->m_allocCount ? pool->m_allocCount : pool->m_peakAllocCount;
                    return ptr;
                }
            }
            gfMemoryPoolBlock* next = block->m_next;
            if (block >= next) {
                break;
            }
            prev = block;
            block = next;
        }
    }
    if (pool->m_debugFlags & 1) {
        OSReport("\x1b[32m gfMemoryPool:  can't alloc memoryblock !!\n");
        gfMemoryPoolBlock* first = pool->m_blocks->m_next;
        u32 totalFree = 0;
        for (gfMemoryPoolBlock* block = first;;) {
            u32 freeSize = block->m_size - sizeof(gfMemoryPoolBlock);
            gfMemoryPoolBlock* next = block->m_next;
            totalFree += freeSize > 0 ? freeSize : 0;
            if (block >= next) {
                break;
            }
            block = next;
        }
        u32 maxFree = 0;
        for (gfMemoryPoolBlock* block = first;;) {
            s32 freeSize = block->m_size - sizeof(gfMemoryPoolBlock);
            freeSize = freeSize > 0 ? freeSize : 0;
            if (maxFree < freeSize) {
                maxFree = freeSize;
            }
            gfMemoryPoolBlock* next = block->m_next;
            if (block >= next) {
                break;
            }
            block = next;
        }
        OSReport("<<gfMemoryPool>>(%s) adr:<0x%08x>,  size:<%d(0x%08x)>\n  total free:  %08x \n  max   free:  %08x \x1b[00m\n",
                 pool->m_name, pool, size, size, totalFree, maxFree);
        fn_800260B4(pool);
        if (pool->m_debugFlags & 0x20) {
            fn_800260B4(pool);
        }
    }
    if (pool->m_debugFlags & 0x10) {
        OSReport("<<gfMemoryPool>> alloc failed halt\n");
        PPCHalt();
    }
    return NULL;
}

void* gfMemoryPool::calloc(void* heapAddr, size_t size, int align) {
    void* ptr = alloc(heapAddr, size, align);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}

u32 gfMemoryPool::getMaxFreeBlockSize() {
    u32 maxSize = 0;
    gfMemoryPoolBlock* block = m_blocks->m_next;
    for (;;) {
        s32 size = block->m_size - sizeof(gfMemoryPoolBlock);
        size = size > 0 ? size : 0;
        if (maxSize < size) {
            maxSize = size;
        }
        gfMemoryPoolBlock* next = block->m_next;
        if (block >= next) {
            return maxSize;
        }
        block = next;
    }
}

void* gfMemoryPoolMemAllocatorAlloc(MEMAllocator* allocator, u32 size);
void gfMemoryPoolMemAllocatorFree(MEMAllocator* allocator, void* block);

MEMAllocatorFuncs lbl_805A0030;

void gfMemoryPool::createMEMAllocator(int heapId, int param) {
    lbl_805A0030.allocFunc = gfMemoryPoolMemAllocatorAlloc;
    lbl_805A0030.freeFunc = gfMemoryPoolMemAllocatorFree;
    m_allocator.funcs = &lbl_805A0030;
    m_allocator.heap = reinterpret_cast<MEMiHeapHead*>(this);
    m_allocator.heapParam1 = heapId;
    m_allocator.heapParam2 = param;
}

void free(void* ptr) {
    freeChecked(ptr);
}

void* gfMemoryPoolMemAllocatorAlloc(MEMAllocator* allocator, u32 size) {
    // MATCH-ONLY: declaring the saved id first gives it the target register.
    u8 id;
    gfMemoryPool* pool = reinterpret_cast<gfMemoryPool*>(allocator->heap);
    id = pool->m_id;
    pool->m_id = 0xFE;
    void* ptr = gfMemoryPool::alloc(pool, size, 0x20);
    pool->m_id = id;
    return ptr;
}

void gfMemoryPoolMemAllocatorFree(MEMAllocator* allocator, void* block) {
    fn_80025F2C(reinterpret_cast<gfMemoryPool*>(allocator->heap), block);
}
