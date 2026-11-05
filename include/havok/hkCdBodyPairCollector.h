#pragma once

#include <havok/hkCdBody.h>

// Collector for body pairs (penetration queries). Vtable: 0x08 dtor, 0x0C addCdBodyPair.
struct hkCdBodyPairCollector {
    virtual ~hkCdBodyPairCollector();
    virtual void addCdBodyPair(const hkCdBody& bodyA, const hkCdBody& bodyB); // 0x0C

    // 0x04 HYPOTHESIS: set by a collector that wants no more pairs; the shape collection agent stops walking its
    // children once it is set.
    hkBool m_earlyOut;
};
