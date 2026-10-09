#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkBvAgent.h>

struct hkCollisionDispatcher;
struct hkPenetrationTarget;

// Collision agent for a height field shape against another shape. Only the query functions are named so far: the
// constructor and the layout of the object are not recovered yet.
struct hkHeightFieldAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    // The non-static query slots forward to the static functions with the this pointer dropped (tail calls).
    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);

    static void staticGetPenetrations(void* a, void* b, void* c, void* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target);
    static void staticLinearCast(void* a, void* b, void* c, void* target, void* d);
};

// HYPOTHESIS: symmetric variant of hkHeightFieldAgent. The class name is the map name
// (hkSymmetricAgentLinearCast<hkHeightFieldAgent>). Each wrapper swaps the first two pair arguments.
struct hkSymmetricAgentLinearCast_18hkHeightFieldAgent : hkHeightFieldAgent {
    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    static void staticGetPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target);
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);
};
