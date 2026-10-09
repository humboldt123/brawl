#pragma once

#include <havok/hkCdPoint.h>

// Collector that keeps the closest collision point reported so far. The 0x04 float mirrors the
// distance of the stored point; the stored point starts at 0x10 and the chain ends of its two
// bodies are kept after it (0x30..0x3C).
struct hkClosestCdPointCollector {
    virtual ~hkClosestCdPointCollector() {}
    virtual void addCdPoint(hkCdPoint* point); // HYPOTHESIS: name from the map

    float m_dist;                 // 0x04
    int unk08;                    // 0x08
    int unk0C;                    // 0x0C
    hkCdPointData m_point;        // 0x10..0x2C, v[7] = distance
    hkCdPointChain* m_lastChainA; // 0x30 last element of chain A (non-null = has hit)
    int m_idA;                    // 0x34 id of chain A head
    hkCdPointChain* m_lastChainB; // 0x38
    int m_idB;                    // 0x3C
};
