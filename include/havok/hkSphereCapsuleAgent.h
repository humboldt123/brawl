#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

struct hkCollisionDispatcher;

// Collision agent for capsule x sphere (capsule first in the pair). Object size 0x10, same layout as
// hkSphereBoxAgent: 0x08 contact manager, 0x0C 16-bit slot released by cleanup when it is not 0xFFFF.
struct hkSphereCapsuleAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC; // 0x0C, set to 0xFFFF by the constructor

    hkSphereCapsuleAgent(hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {
        unkC = 0xFFFF;
    }
    virtual ~hkSphereCapsuleAgent() {} // inline: the symmetric destructor calls it without a bl

    // Registers the capsule/sphere agent functions with the dispatcher (two pairs, see the .cpp).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // HYPOTHESIS: the first three parameters are the two bodies and the collision input; only the
    // contact manager (last parameter) is stored.
    static hkSphereCapsuleAgent* createCapsuleSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    // Creates the symmetric (flipped) variant.
    static hkSphereCapsuleAgent* createSphereCapsuleAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) __attribute__((never_inline));

    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
};

// Flipped (symmetric) variant of the capsule agent; see hkSphereBoxAgent.h for the naming convention.
// The destructor is defined in this translation unit.
struct hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_ : hkSphereCapsuleAgent {
    hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_(hkContactMgr* contactMgr) : hkSphereCapsuleAgent(contactMgr) {}
    virtual ~hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_();

    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void updateShapeCollectionFilter() {}

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
};
