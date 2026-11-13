#pragma once

#include <havok/hkCdPoint.h>

// Collector that keeps the closest contact reported so far, without the chain bookkeeping of
// hkClosestCdPointCollector. 0x08 flags that a point has been stored.
// HYPOTHESIS: not derived from hkCdPointCollector here, although the layout (best distance at 0x04) and the slots
// agree. Deriving keeps the iterative linear cast's collector address in a register as in the target, but the base's
// hkSimdReal store then turns copy propagation off in hkBvAgent's linear casts, which the target does not.
struct hkSimpleClosestContactCollector {
    HK_DECLARE_SIZED_DELETE(0x1d)
    hkSimpleClosestContactCollector() { reset(); }
    virtual ~hkSimpleClosestContactCollector() {}
    // The iterative linear cast reuses one collector per query.
    void reset() {
        m_hit = false;
        m_point.m_normal.w = HK_REAL_MAX;
        m_dist = HK_REAL_MAX;
    }
    virtual void addCdPoint(hkCdPoint* point); // HYPOTHESIS: name from the map

    hkReal m_dist;          // 0x04
    hkBool m_hit;           // 0x08
    int unk0C;              // 0x0C
    hkContactPoint m_point; // 0x10..0x2F (distance in m_normal.w)
};
