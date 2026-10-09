#pragma once

#include <havok/hkBase.h>

// HYPOTHESIS: link of a body chain attached to a collision point. The chain is walked through
// the next pointer at 0x0C; the 32-bit id at 0x04 is read from the head element.
struct hkCdPointChain {
    int unk00;               // 0x00
    int m_id;                // 0x04 HYPOTHESIS
    int unk08;               // 0x08
    hkCdPointChain* m_next;  // 0x0C
};

// Collision point handed to the collector callbacks. The first eight floats are copied verbatim
// by the collectors; v[7] is the distance (its last component). The chains are the two bodies'
// chains of the pair.
struct hkCdPointData {
    float f0, f1, f2, f3, f4, f5, f6, f7; // 0x00..0x1C, f7 = distance HYPOTHESIS
};

struct hkCdPoint {
    hkCdPointData data;       // 0x00..0x1C
    hkCdPointChain* m_chainA; // 0x20
    hkCdPointChain* m_chainB; // 0x24
};
