#pragma once

#include <havok/hkCdBody.h>

// Collector that only records that a body pair was reported.
struct hkFlagCdBodyPairCollector {
    hkFlagCdBodyPairCollector() : m_hit(0) {}
    virtual ~hkFlagCdBodyPairCollector() {}
    virtual void addCdBodyPair(hkCdBody* bodyA, hkCdBody* bodyB); // HYPOTHESIS: parameters unused

    hkBool m_hit; // 0x04
};
