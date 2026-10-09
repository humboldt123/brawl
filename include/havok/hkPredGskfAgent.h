#pragma once

#include <havok/hkGskfAgent.h>

struct hkCollisionDispatcher;
struct hkContactMgr;

// Predictive variant of the GSK point agent (map name hkPredGskfAgent). It shares the hkGskfAgent layout (0x80 bytes);
// the creation function replaces the vtable after the base constructor.
struct hkPredGskfAgent : hkGskfAgent {
    // Registers the create/static-query functions for the convex x convex pair with the dispatcher.
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // HYPOTHESIS: the third parameter is unused (as in hkGskfAgent::createGskfAgent).
    static hkGskBaseAgent* createPredGskfAgent(hkCdBody* bodyA, hkCdBody* bodyB, void* unk2, hkContactMgr* contactMgr);
};
