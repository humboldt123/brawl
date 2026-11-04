#pragma once
// Local BrawlHeaders shadow: adds the inline constructor that takes the start routine,
// priority, stack size and heap (gfCollectionIO request and drive-status threads).

#include <StaticAssert.h>
#include <revolution/OS/OSThread.h>
#include <sr/sr_common.h>
#include <types.h>

class gfRunnable {
public:
    virtual void run() = 0;
};
static_assert(sizeof(gfRunnable) == 0x4, "Class is the wrong size!");

class gfCollectionIO;

class gfThread : public gfRunnable {
    // HYPOTHESIS: the collection IO polls and resumes its worker threads directly.
    friend class gfCollectionIO;
protected:
    u8 unk4[0x4];
    OSThread m_thread;          // 0x8
    u32 m_stackSize;            // 0x320
    void* m_stackLow;           // 0x324
    void* m_stackHi;            // 0x328
    const char* m_name;
    u32 m_priority;             // 0x330
    u8 unk334[0x4];
    u16 m_flags;                // 0x338
    gfRunnable* m_startRoutine; // 0x33C
public:
    gfThread() {
        createThread(this, 0x1F, 0x1000, Heaps::Thread);
    }
    gfThread(gfRunnable* startRoutine, u32 priority, u32 stackSize, Heaps::HeapType heap) {
        createThread(startRoutine, priority, stackSize, heap);
    }
    virtual void run() { }
    ~gfThread();
    // HYPOTHESIS: names for the inline thread operations sndSystem performs on its worker threads.
    void resume() { OSResumeThread(&m_thread); }
    void setPriority(u32 priority) {
        m_priority = priority;
        OSSetThreadPriority(&m_thread, priority);
    }
    void createThread(gfRunnable* startRoutine, u32 priority, u32 stackSize, Heaps::HeapType heap);
    static void* startThread(void* arg);
};
static_assert(sizeof(gfThread) == 0x340, "Class is the wrong size!");
