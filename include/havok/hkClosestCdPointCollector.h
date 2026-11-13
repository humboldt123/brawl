#pragma once

#include <havok/hkCdPoint.h>

// Collector that keeps the closest collision point reported so far. The early-out distance (0x04) follows the
// distance of the stored point; the stored point starts at 0x10 and the chain ends of its two
// bodies are kept after it (0x30..0x3C).
struct hkClosestCdPointCollector : hkCdPointCollector {
    hkClosestCdPointCollector() {
        reset();
    }
    // No point yet.
    void reset() {
        m_rootA = 0;
        m_point.m_normal.w = HK_REAL_MAX;
        m_earlyOutDistance = HK_REAL_MAX;
    }
    virtual ~hkClosestCdPointCollector() {}
    virtual void addCdPoint(hkCdPoint* point); // HYPOTHESIS: name from the map

    hkBool hasHit() const {
        return m_rootA != 0;
    }

    int unk08;                    // 0x08
    int unk0C;                    // 0x0C
    hkContactPoint m_point;       // 0x10..0x2F (distance in m_normal.w)
    const hkCdBody* m_rootA;      // 0x30 root of body A of the stored point (non-null = has hit)
    u32 m_shapeKeyA;              // 0x34 shape key of body A
    const hkCdBody* m_rootB;      // 0x38
    u32 m_shapeKeyB;              // 0x3C
};
