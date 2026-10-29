#pragma once

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <sr/sr_common.h>
#include <types.h>

class gfCryptoESP : public gfThread {
public:
    u8 m_iv[0x10];           // 0x340
    const void* m_src;       // 0x350
    u32 m_srcSize;           // 0x354
    Heaps::HeapType m_heap;  // 0x358
    // HYPOTHESIS: 1 encrypts, 2 decrypts, 0 is idle.
    s32 m_mode;              // 0x35C
    bool m_isDone;           // 0x360
    s32 m_result;            // 0x364
    void* m_dst;             // 0x368
    u32 m_dstSize;           // 0x36C
    // HYPOTHESIS: set by the owner that keeps the output buffer for later release.
    bool m_freeDst;          // 0x370

    gfCryptoESP();
    ~gfCryptoESP();
    // HYPOTHESIS: parameter types from the callee's register use; the map has none.
    bool encrypt(const void* src, u32 size, Heaps::HeapType heap);
    bool decrypt(const void* src, u32 size, Heaps::HeapType heap);
    virtual void run();
};
static_assert(sizeof(gfCryptoESP) == 0x378, "Class is the wrong size!");
