// Local BrawlHeaders shadow: adds the inline constructor that sndSystem::sndSystem expands (the worker thread that
// opens the archive header and sets up the buffers).
#pragma once

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <revolution/OS/OSMutex.h>

class sndInitThread : public gfThread {
    mutable OSMutex m_mutex;
    bool m_initDone;
    bool m_loadDone;
public:
    sndInitThread() : gfThread(this, 0x1F, 0x1000, Heaps::Sound) {
        OSInitMutex(&m_mutex);
        m_initDone = false;
        m_loadDone = false;
    }
    void run();
    bool isFinishInitialize() const;
    bool isFinishLoad() const;
};
static_assert(sizeof(sndInitThread) == 0x360, "Class is the wrong size!");
