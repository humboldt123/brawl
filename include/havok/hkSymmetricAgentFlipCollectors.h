#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>
#include <havok/hkCdPoint.h>

// Collector wrappers used by the symmetric (shape-swapping) agent wrappers. A wrapper builds one of these on
// its stack and passes its address as the collector argument, so the results reach the original collector
// with the sides flipped. Layout: 0x00 vtable, 0x04 early-out flag / distance, 0x08 wrapped collector.
// Which wrapper uses which class: from the vtables the wrappers store (their 0x0C slot is the map's
// hkSymmetricAgentFlip*Collector::addCd* function).

// HYPOTHESIS: base of the body pair collectors; the constructor clears the early-out flag.
struct hkSymmetricAgentFlipBodyCollectorBase {
    hkSymmetricAgentFlipBodyCollectorBase() { unk4 = 0; }
    virtual void addCdBodyPair(void* bodyA, void* bodyB) = 0; // 0x0C HYPOTHESIS (the dtor is slot 0x08)
    u8 unk4; // 0x04 HYPOTHESIS: early-out flag
};

// Penetration queries (body pairs). Vtable lbl_80487178.
struct hkSymmetricAgentFlipBodyCollector : hkSymmetricAgentFlipBodyCollectorBase {
    void* unk8; // 0x08 wrapped collector
    hkSymmetricAgentFlipBodyCollector(void* collector) : unk8(collector) {}
    virtual void addCdBodyPair(void* bodyA, void* bodyB);
};

// HYPOTHESIS: base of the point collectors; the constructor sets the early-out distance to 0.
struct hkSymmetricAgentFlipPointCollectorBase {
    hkSymmetricAgentFlipPointCollectorBase() : unk4(0.0f) {}
    virtual void addCdPoint(void* point) = 0; // 0x0C HYPOTHESIS (the dtor is slot 0x08)
    float unk4; // 0x04 HYPOTHESIS: early-out distance
};

// Closest-point queries. Vtable lbl_80487198.
struct hkSymmetricAgentFlipCollector : hkSymmetricAgentFlipPointCollectorBase {
    void* unk8; // 0x08 wrapped collector
    hkSymmetricAgentFlipCollector(void* collector) : unk8(collector) {}
    virtual void addCdPoint(void* point);
};

// Linear casts (0x30 bytes). Layout from the symmetric linearCast wrappers: the early-out distance starts at
// HK_REAL_MAX, then the cast path of the unswapped query and the wrapped collector.
struct hkSymmetricAgentFlipCastCollector : hkCdPointCollector {
    hkSymmetricAgentFlipCastCollector(const hkVector4& path, void* collector) : m_path(path), m_collector(collector) {}
    virtual void addCdPoint(hkCdPoint* point);

    hkVector4 m_path;  // 0x10
    void* m_collector; // 0x20
};
