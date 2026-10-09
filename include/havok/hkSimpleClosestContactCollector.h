#pragma once

#include <havok/hkCdPoint.h>

// Collector that keeps the closest contact reported so far, without the chain bookkeeping of
// hkClosestCdPointCollector. 0x08 flags that a point has been stored.
struct hkSimpleClosestContactCollector {
    virtual ~hkSimpleClosestContactCollector() {}
    virtual void addCdPoint(hkCdPoint* point); // HYPOTHESIS: name from the map

    float m_dist;     // 0x04
    hkBool m_hit;     // 0x08
    int unk0C;        // 0x0C
    hkCdPointData m_point; // 0x10..0x2C, v[7] = distance
};
