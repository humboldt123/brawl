#pragma once

#include <havok/hkCdBody.h>

// Collector that only records that a body pair was reported.
struct hkFlagCdBodyPairCollector {
    virtual ~hkFlagCdBodyPairCollector() {}
    virtual void addCdBodyPair(hkCdBody* bodyA, hkCdBody* bodyB); // HYPOTHESIS: parameters unused

    u8 m_hit; // 0x04
};
