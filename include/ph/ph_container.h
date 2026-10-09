#pragma once

#include <havok/hkBase.h>
#include <havok/hkMemory.h>
#include <havok/hkStream.h>

// Array header used by phContainer and phActor: data pointer, element count, capacity with a flag in bit 31.
struct phContainerArray {
    void* m_data;             // 0x00
    int m_size;               // 0x04
    u32 m_capacityAndFlags;   // 0x08 HYPOTHESIS: bit 31 = data not owned (constructor stores 0x80000000)
};

// Physics container (plain struct, no vptr). Three arrays at 0x08, 0x14, 0x20 (partly recovered).
struct phContainer {
    phContainer();
    ~phContainer();
    void applyData(); // HYPOTHESIS: signature not recovered yet
    void relAll();    // HYPOTHESIS: signature not recovered yet

    u8 unk00[0x08];              // 0x00
    phContainerArray m_arrayA;   // 0x08
    phContainerArray m_arrayB;   // 0x14
    phContainerArray m_arrayC;   // 0x20
    u32 unk2C;                   // 0x2C
    u32 unk30;                   // 0x30
    u32 unk34;                   // 0x34
    u32 unk38;                   // 0x38
    u32 unk3C;                   // 0x3C
};

// Stream reader over an in-memory buffer (the target's hkStreamReader subclass).
// Virtual slots and the destructor's size-based deallocation (class 0x18) come from the base and macro.
struct phStreamReader : hkStreamReader {
    phStreamReader();
    virtual ~phStreamReader();
    virtual hkBool isOk() const;
    virtual int read(void* buf, int nbytes);

    HK_DECLARE_REF_ALLOCATOR(HK_MEMORY_CLASS_STREAM)

    u8* m_buffer; // 0x08 HYPOTHESIS: source bytes
    u32 unkC;     // 0x0C
    u32 m_pos;    // 0x10 HYPOTHESIS: read offset; bit 31 cleared means ok
};
