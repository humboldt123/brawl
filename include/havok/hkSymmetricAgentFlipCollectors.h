#pragma once

#include <havok/hkBase.h>

// Collector wrappers used by the symmetric (shape-swapping) agent wrappers. A wrapper builds one of these on
// its stack and passes its address as the collector argument, so the results reach the original collector
// with the sides flipped. Layout: 0x00 vtable, 0x04 flag/fraction, 0x08 wrapped collector.
// HYPOTHESIS: names come from the Havok map (hkSymmetricAgentFlip*Collector__addCdPoint); the vtable slots and
// the addCdPoint bodies are not recovered yet.

// Penetration collector (flag 0 at 0x04).
struct hkSymmetricAgentFlipCollector {
    virtual void addCdPoint(void* point); // HYPOTHESIS
    u8 unk4;    // 0x04 HYPOTHESIS: zero for penetration queries
    void* unk8; // 0x08 wrapped collector

    hkSymmetricAgentFlipCollector(void* collector) {
        unk4 = 0;
        unk8 = collector;
    }
};

// Closest-points collector (0.0f fraction at 0x04).
struct hkSymmetricAgentFlipCastCollector {
    virtual void addCdPoint(void* point); // HYPOTHESIS
    float unk4; // 0x04 HYPOTHESIS: zero for closest-points queries
    void* unk8; // 0x08 wrapped collector

    hkSymmetricAgentFlipCastCollector(void* collector) : unk4(0.0f), unk8(collector) {}
};
