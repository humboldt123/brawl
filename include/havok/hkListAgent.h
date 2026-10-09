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
    static void* operator new(unsigned long, void* where) { return where; } // placement new for the factories

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

    // Constructor (map name hkListAgent____ct, not written yet). Sets the vptr of hkListAgent itself.
    hkListAgent(void* a, void* b, void* c, int unk8Value);

    // Factories: createListAAgent swaps the first two pair arguments and uses the variant vtable.
    static hkListAgent* createListAAgent(void* a, void* b, void* c, int d);
    static hkListAgent* createListBAgent(void* a, void* b, void* c, int d);

    static void staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticLinearCast(void* a, void* b, void* c, hkPenetrationTarget* target, void* d);
};

// HYPOTHESIS: agent variant created by createListAAgent (vtable lbl_80486A30). The class adds no members; the
// constructor is the base constructor followed by the vptr store.
struct hkListAgentVariant : hkListAgent {
    hkListAgentVariant(void* a, void* b, void* c, int unk8Value) : hkListAgent(a, b, c, unk8Value) {}
};
