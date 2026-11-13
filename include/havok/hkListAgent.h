#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkPhantomAgent.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkBvAgent.h>

// List collision agent (map name hkListAgent). Derives from hkCollisionAgent: the vtable of the
// symmetric variant (lbl_80486A30) has the base slot order. Only invalidateTim/warpTime are named
// in that vtable among the overrides; removePoint, commitPotential and createZombie stay the base
// slots. The agent track at 0x10 is what invalidateTim/warpTime/cleanup forward to (object size 0x20).
struct hkListAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for the factories

    int unkC;             // 0x0C copied into the sub-object's filter call by cleanup (HYPOTHESIS)
    hkAgent1nTrack m_agentTrack; // 0x10 the child agents (hkAgent1nMachine), destroyed by the deleting destructor

    virtual ~hkListAgent() {}

    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);                  // 0x10 HYPOTHESIS
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);                 // 0x14 HYPOTHESIS
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);              // 0x18 HYPOTHESIS
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);                               // 0x1C HYPOTHESIS
    virtual void cleanup();                                                                 // 0x20 HYPOTHESIS
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);                    // 0x24 HYPOTHESIS
    virtual void invalidateTim(void* arg);                                                  // 0x28 HYPOTHESIS
    virtual void warpTime(float t0, float t1, void* arg);                                   // 0x2C HYPOTHESIS

    // Constructor (map name hkListAgent____ct). Sets the vptr of hkListAgent itself. Called, not inlined, by the
    // factories in its own unit.
    hkListAgent(void* a, void* b, void* c, int unk8Value) __attribute__((never_inline));

    // Registers the list agent for list x anything pairs in both orders.
    static void registerAgent(void* dispatcher);

    // Factories: createListAAgent swaps the first two pair arguments and uses the variant vtable.
    static hkListAgent* createListAAgent(void* a, void* b, void* c, int d);
    static hkListAgent* createListBAgent(void* a, void* b, void* c, int d);

    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, hkPenetrationTarget* target, void* d);
};

// hkSymmetricAgent<hkListAgent> (map name; vtable lbl_80486A30, created by createListAAgent and by
// hkConvexListAgent::createListConvexAgent): the list agent with the bodies swapped, over
// hkSymmetricAgentLinearCast<hkListAgent>. Its deleting destructor (in hkConvexListAgent.o) tests `this` once per level.
struct hkSymmetricAgent_11hkListAgent_ : hkSymmetricAgentLinearCast<hkListAgent> {
    hkSymmetricAgent_11hkListAgent_(void* a, void* b, void* c, int unk8Value)
        : hkSymmetricAgentLinearCast<hkListAgent>(a, b, c, unk8Value) {}
    virtual ~hkSymmetricAgent_11hkListAgent_() {}
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};
