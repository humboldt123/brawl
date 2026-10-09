#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkGskManifold.h>

struct hkCollisionDispatcher;

// Predictive GSK agent for cylinders (map name hkPredGskCylinderAgent3). Only the registration is declared so far.
struct hkPredGskCylinderAgent3 : hkCollisionAgent {
    hkGskManifold m_manifold; // 0x0C HYPOTHESIS: same manifold layout as hkPredGskAgent3

    static void registerAgent3(hkCollisionDispatcher* dispatcher);
};
