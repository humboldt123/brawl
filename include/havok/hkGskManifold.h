#pragma once

#include <havok/hkBase.h>

// Contact manifold of the GSK agents (map name hkGskManifold), embedded in the agent at 0x0C. Its first three bytes
// are element counts: the sizes of the two 2-byte lists and of the 8-byte list that follow the header.
struct hkGskManifold {
    u8 m_countA; // 0x00 HYPOTHESIS: count of 2-byte entries
    u8 m_countB; // 0x01 HYPOTHESIS: count of 2-byte entries
    u8 m_countC; // 0x02 HYPOTHESIS: count of 8-byte entries

    // Size in bytes of the header plus the lists (header of 4 bytes).
    u32 getTotalSizeInBytes() const;
};
