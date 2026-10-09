#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkGskManifold.h>

struct hkCollisionDispatcher;

// Predictive GSK agent (map name hkPredGskAgent3). HYPOTHESIS: derives from hkCollisionAgent; the GSK manifold is embedded
// at 0x0C. The point-list functions take the context as first argument (it is not the agent) and the agent second, so they
// are static.
struct hkPredGskAgent3 : hkCollisionAgent {
    hkGskManifold m_manifold; // 0x0C

    // Registers the create/static-query functions for the predictive GSK agent with the dispatcher.
    static void registerAgent3(hkCollisionDispatcher* dispatcher);

    // Point-list operations used by the collision dispatcher (HYPOTHESIS: the first argument is a context pointer
    // that is unused, or is a destination manifold in cleanup).
    static void* cleanup(hkGskManifold* dst, hkPredGskAgent3* agent, void* arg);
    static void destroy(void* ctx, hkPredGskAgent3* agent, void* arg);
    static void removePoint(void* ctx, hkPredGskAgent3* agent, u16 key);
    static void commitPotential(void* ctx, hkPredGskAgent3* agent, u16 key);
    static void createZombie(void* ctx, hkPredGskAgent3* agent, u16 key);
};
