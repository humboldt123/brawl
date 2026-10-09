#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

struct hkCollisionDispatcher;

// Collision agent for box x sphere (box first in the pair). Object size 0x10, same layout as
// hkSphereSphereAgent: 0x08 contact manager, 0x0C 16-bit slot released by cleanup when it is not 0xFFFF.
struct hkSphereBoxAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC; // 0x0C, set to 0xFFFF by the constructor

    hkSphereBoxAgent(hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {
        unkC = 0xFFFF;
    }
    virtual ~hkSphereBoxAgent() {} // inline: the symmetric destructors call it without a bl

    // Registers the sphere/box agent functions with the dispatcher (two pairs, see the .cpp).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // HYPOTHESIS: the first three parameters are the two bodies and the collision input; only the
    // contact manager (last parameter) is stored.
    static hkSphereBoxAgent* createBoxSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    // Creates the symmetric (flipped) variant, see hkSymmetricAgentLinearCast below.
    static hkSphereBoxAgent* createSphereBoxAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void processCollision(void* unk0, void* unk1, void* unk2);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) __attribute__((never_inline));

    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
};

// Flipped (symmetric) variant of the box agent: the shapes are swapped and the collector is wrapped
// (see hkSymmetricAgentFlipCollectors.h). The class name follows the Havok TU map, which flattens the
// template argument (hkSymmetricAgentLinearCast<hkSphereBoxAgent>). HYPOTHESIS: the base is the box agent.
// processCollision, staticLinearCast and the destructor are not written yet.
struct hkSymmetricAgentLinearCast_16hkSphereBoxAgent_ : hkSphereBoxAgent {
    hkSymmetricAgentLinearCast_16hkSphereBoxAgent_(hkContactMgr* contactMgr) : hkSphereBoxAgent(contactMgr) {}

    // The first out-of-line virtual (getPenetrations) is the key function, which puts the vtable in this unit.
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    // Destructor defined in hkSphereCapsuleAgent.cpp (map places it there).
    virtual ~hkSymmetricAgentLinearCast_16hkSphereBoxAgent_();
    virtual void updateShapeCollectionFilter() {}

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
};
