#pragma once

#include <havok/hkBase.h>

// Secondary line constraint (a chain of nodes driven by the 2ndary controller). Only the members touched by
// the destructor are recovered so far; the rest of the layout is unknown.
struct ph2ndaryLine {
    ph2ndaryLine();   // not written yet
    ~ph2ndaryLine();

    u8 unk00[0x1F8];                // 0x00
    struct phOwnedBlock* m_block;   // 0x1F8 HYPOTHESIS: owned object, deleted when its first word is 0
    void* m_arrayData;              // 0x1FC element array (8-byte elements), freed with class 0x15
    int m_arrayCount;               // 0x200
    u32 m_arrayCapacityFlags;       // 0x204 bit 31 = data not owned
};
