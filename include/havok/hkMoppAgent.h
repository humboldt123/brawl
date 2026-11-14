#pragma once

#include <havok/hkBvTreeAgent.h>

struct hkContactMgr;

// Collision agent for a MOPP shape (object layout of hkBvTreeAgent). Only the constructor and the symmetric wrappers
// are recovered so far: the query functions are inherited from hkBvTreeAgent, and the vtable is the one set by the
// constructor after the base constructor.
struct hkMoppAgent : hkBvTreeAgent {
    hkMoppAgent(hkContactMgr* contactMgr) __attribute__((never_inline));
    virtual ~hkMoppAgent() {}
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d); // not written yet
    // Factory for MOPP x MOPP pairs: the tree with the smaller code is the shape side.
    static hkCollisionAgent* createBvBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                             hkContactMgr* contactMgr);
};

// HYPOTHESIS: symmetric variant of hkMoppAgent. The class name is the map name (hkSymmetricAgentLinearCast<hkMoppAgent>).
// Each wrapper swaps the first two pair arguments and forwards to the inherited hkBvTreeAgent query.
struct hkSymmetricAgentLinearCast_11hkMoppAgent : hkMoppAgent {
    hkSymmetricAgentLinearCast_11hkMoppAgent(hkContactMgr* contactMgr) : hkMoppAgent(contactMgr) {}

    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);
};

// HYPOTHESIS: hkSymmetricAgent<hkMoppAgent> (map name); only its linear cast is written.
struct hkSymmetricAgent_11hkMoppAgent_ : hkMoppAgent {
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};
