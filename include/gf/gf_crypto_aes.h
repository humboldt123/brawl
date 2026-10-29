#pragma once

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <sr/sr_common.h>
#include <types.h>

class gfCryptoAES : public gfThread {
public:
    u8 m_iv[0x10];           // 0x340
    u8 m_key[0x10];          // 0x350
    const void* m_src;       // 0x360
    u32 m_srcSize;           // 0x364
    Heaps::HeapType m_heap;  // 0x368
    // HYPOTHESIS: 1 encrypts, 2 decrypts, 0 is idle.
    s32 m_mode;              // 0x36C
    bool m_isDone;           // 0x370
    s32 m_result;            // 0x374
    void* m_dst;             // 0x378
    u32 m_dstSize;           // 0x37C
    // HYPOTHESIS: set by the owner that keeps the output buffer for later release.
    bool m_freeDst;          // 0x380

    gfCryptoAES();
    ~gfCryptoAES();
    // HYPOTHESIS: parameter types from the callee's register use; the map has none.
    bool encrypt(const void* src, u32 size, Heaps::HeapType heap);
    bool decrypt(const void* src, u32 size, Heaps::HeapType heap);
    virtual void run();
};
static_assert(sizeof(gfCryptoAES) == 0x388, "Class is the wrong size!");
