#pragma once

#include <havok/hkBase.h>

struct hkCollisionDispatcher;
struct hkAgent3Input;
struct hkAgentEntry;
struct hkContactMgr;
struct hkVector4;

// Box x box agent of the agent3 (stream) machine. Its functions are static: the agent data in the track sector
// starts with an hkBoxBoxManifold (0x50 bytes of agent data).
struct hkBoxBoxAgent3 {
    static void registerAgent3(hkCollisionDispatcher* dispatcher);
    static void* create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData);
    // Removes every contact point the manifold still holds from the contact manager and returns the end of the data.
    // (never inlined: destroy calls it out of line)
    static void* cleanup(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr) __attribute__((never_inline));
    static void removePoint(hkAgentEntry* entry, void* agentData, u16 key);
    // Gives the first free (0xFFFF) manifold point the contact point id.
    static void commitPotential(hkAgentEntry* entry, void* agentData, u16 key);
    // Empty in the original (0x802FB45C).
    static void createZombie(hkAgentEntry* entry, void* agentData, u16 key);
    static void destroy(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr);
    // Builds the box-box detector on the stack and refreshes the manifold; returns the end of the agent data.
    static void* process(const struct hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                         hkVector4* separatingNormal, struct hkProcessCollisionOutput& result);
};
