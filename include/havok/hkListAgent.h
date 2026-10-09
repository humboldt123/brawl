#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkPhantomAgent.h>

// List collision agent (map name hkListAgent). The layout is not recovered beyond the embedded
// object at 0x10 that invalidateTim/warpTime forward to. The virtual slots follow the layout that
// hkBvAgent uses (see hkBvAgent.h); the names come from the map, the order is a HYPOTHESIS.
struct hkListAgent : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    virtual ~hkListAgent() {}

    virtual void processCollision();                                                        // 0x10 HYPOTHESIS
    virtual void getClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target);  // 0x14 HYPOTHESIS
    virtual void linearCast(void* a, void* b, void* c, hkPenetrationTarget* target, void* d); // 0x18 HYPOTHESIS
    virtual void getPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target);  // 0x1C HYPOTHESIS
    virtual void cleanup();                                                                 // 0x20 HYPOTHESIS
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);                    // 0x24 HYPOTHESIS
    virtual void invalidateTim(void* arg);                                                  // 0x28 HYPOTHESIS
    virtual void warpTime(float t0, float t1, void* arg);                                   // 0x2C HYPOTHESIS
    virtual void removePoint(void* arg);                                                    // 0x30 HYPOTHESIS
    virtual void commitPotential(void* arg);                                                // 0x34 HYPOTHESIS
    virtual void createZombie(void* arg);                                                   // 0x38 HYPOTHESIS

    static void staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticLinearCast(void* a, void* b, void* c, hkPenetrationTarget* target, void* d);
};
