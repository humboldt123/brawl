#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkBoxBoxManifold.h>
#include <havok/hkCdBody.h>

struct hkCollisionDispatcher;

// Collision agent for box x box pairs (and the box side of box x other pairs) with a contact manifold. Object size 0x60.
// The 0x08 word (inherited) holds the contact manager; the manifold sits at 0x10 (its point count is at 0x31).
struct hkBoxBoxAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u8 unkC[0x10 - 0x0C]; // 0x0C not identified yet
    hkBoxBoxManifold m_manifold; // 0x10
    u8 unk34[0x60 - 0x34];       // 0x34 not identified yet

    hkBoxBoxAgent(hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {}
    virtual ~hkBoxBoxAgent();

    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Creates a box agent when the pair is close to both boxes' sizes, otherwise a GSK agent (hkGskfAgent).
    // HYPOTHESIS: the third parameter is the collision input (tolerance at 0x08, read as a float).
    static hkCollisionAgent* createBoxBoxAgent(hkCdBody* bodyA, hkCdBody* bodyB, const void* input, hkContactMgr* contactMgr);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);

    virtual void cleanup();
    virtual void processCollision(void* unk0, void* unk1, void* unk2); // 0x1C, not written yet
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
};
