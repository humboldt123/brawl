#pragma once

#include <havok/hkCdBody.h>
#include <havok/hkMemory.h>

// Collector that only records that a body pair was reported.
struct hkFlagCdBodyPairCollector {
    HK_DECLARE_SIZED_DELETE(0x1d)
    hkFlagCdBodyPairCollector() : m_hit(0) {}
    virtual ~hkFlagCdBodyPairCollector() {}
    virtual void addCdBodyPair(hkCdBody* bodyA, hkCdBody* bodyB); // HYPOTHESIS: parameters unused

    hkBool m_hit; // 0x04
};
