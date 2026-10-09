#pragma once

#include <havok/hkBvTreeAgent.h>

struct hkContactMgr;

// Collision agent for a MOPP shape (object layout of hkBvTreeAgent). Only the constructor and the symmetric wrappers
// are recovered so far: the query functions are inherited from hkBvTreeAgent, and the vtable is the one set by the
// constructor after the base constructor.
struct hkMoppAgent : hkBvTreeAgent {
    hkMoppAgent(hkContactMgr* contactMgr) __attribute__((never_inline));
    virtual ~hkMoppAgent() {}
};

// HYPOTHESIS: symmetric variant of hkMoppAgent. The class name is the map name (hkSymmetricAgentLinearCast<hkMoppAgent>).
// Each wrapper swaps the first two pair arguments and forwards to the inherited hkBvTreeAgent query.
struct hkSymmetricAgentLinearCast_11hkMoppAgent : hkMoppAgent {
    hkSymmetricAgentLinearCast_11hkMoppAgent(hkContactMgr* contactMgr) : hkMoppAgent(contactMgr) {}

    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    static void staticGetPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target);
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);
};
