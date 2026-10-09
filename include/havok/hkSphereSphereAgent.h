#pragma once

#include <havok/hkCollisionAgent.h>

struct hkCdBody;
struct hkCollisionDispatcher;

// Collision agent for two spheres. Object size 0x10. The 0x08 word (inherited from hkCollisionAgent) holds the
// contact manager passed to the create function; 0x0C is a 16-bit slot that cleanup releases when it is not 0xFFFF.
struct hkSphereSphereAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC; // 0x0C, set to 0xFFFF by the constructor

    hkSphereSphereAgent(hkContactMgr* contactMgr) {
        unk8 = (int)contactMgr;
        unkC = 0xFFFF;
    }
    virtual ~hkSphereSphereAgent();

    // Registers the create/static-query functions for sphere x sphere with the dispatcher.
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // HYPOTHESIS: the first three parameters are the two bodies and the collision input; only the
    // contact manager (last parameter) is stored.
    static hkSphereSphereAgent* createSphereSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void processCollision(void* unk0, void* unk1, void* unk2);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);

    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
};
