#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkGskManifold.h>
#include <havok/hkPredGskAgent3.h>

struct hkCollisionDispatcher;
struct hkAgent3ProcessInput;
struct hkAgentEntry;
struct hkProcessCollisionOutput;

// Predictive GSK agent for cylinders (map name hkPredGskCylinderAgent3): the predictive GSK agent with each cylinder
// replaced by a capsule while it runs.
struct hkPredGskCylinderAgent3 : hkCollisionAgent {
    hkGskManifold m_manifold; // 0x0C HYPOTHESIS: same manifold layout as hkPredGskAgent3

    static void registerAgent3(hkCollisionDispatcher* dispatcher);
    // Flags the bodies that are cylinders in the entry, then creates the predictive GSK agent.
    static void* create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData);
    // Runs the predictive GSK agent with each cylinder that is nearly parallel to the separating plane replaced by a
    // capsule (hysteresis between the two cosine limits) and returns the end of the agent data.
    static void* process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                         hkVector4* separatingNormal, hkProcessCollisionOutput& output);
};
