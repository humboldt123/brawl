#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkPhantomAgent.h>

// List collision agent (map name hkListAgent). Derives from hkCollisionAgent: the vtable of the
// symmetric variant (lbl_80486A30) has the base slot order. Only invalidateTim/warpTime are named
// in that vtable among the overrides; removePoint, commitPotential and createZombie stay the base
// slots. The layout is not recovered beyond the embedded object at 0x10 that invalidateTim/warpTime
// forward to.
struct hkListAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    int unkC;             // 0x0C copied into the sub-object's filter call by cleanup (HYPOTHESIS)
    u8 m_subObject[0x10]; // 0x10 embedded object that invalidateTim/warpTime/cleanup forward to

    virtual ~hkListAgent() {}

    virtual void getPenetrations(void* a, void* b, void* c, void* target);                  // 0x10 HYPOTHESIS
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);                 // 0x14 HYPOTHESIS
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);              // 0x18 HYPOTHESIS
    virtual void processCollision(void* a, void* b, void* c);                               // 0x1C HYPOTHESIS
    virtual void cleanup();                                                                 // 0x20 HYPOTHESIS
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);                    // 0x24 HYPOTHESIS
    virtual void invalidateTim(void* arg);                                                  // 0x28 HYPOTHESIS
    virtual void warpTime(float t0, float t1, void* arg);                                   // 0x2C HYPOTHESIS

    static void staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticLinearCast(void* a, void* b, void* c, hkPenetrationTarget* target, void* d);
};
