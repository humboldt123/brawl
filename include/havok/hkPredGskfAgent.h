#pragma once

#include <havok/hkGskfAgent.h>

struct hkCollisionDispatcher;
struct hkContactMgr;

// Predictive variant of the GSK point agent (map name hkPredGskfAgent). It shares the hkGskfAgent layout (0x80 bytes);
// the creation function replaces the vtable after the base constructor.
struct hkPredGskfAgent : hkGskfAgent {
    hkPredGskfAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkContactMgr* contactMgr)
        : hkGskfAgent(bodyA, bodyB, contactMgr) {}

    // Registers the create/static-query functions for the convex x convex pair with the dispatcher.
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Allocates a hkPredGskfAgent (0x80 bytes) when contactMgr is set, otherwise a hkGskBaseAgent (0x30 bytes).
    // HYPOTHESIS: the input is unused (as in hkGskfAgent::createGskfAgent).
    static hkGskBaseAgent* createPredGskfAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                               hkContactMgr* contactMgr);
};
