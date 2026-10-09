#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

struct hkCollisionDispatcher;

// Collision agent for triangle x sphere (triangle first in the pair). Object size 0x20: the 0x08 word is the
// contact manager, 0x0C a 16-bit slot released by cleanup when it is not 0xFFFF, and 0x10 a 16-byte sub-object
// that the constructor fills from the triangle argument (fn_80325184, not recovered yet).
struct hkSphereTriangleAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC;     // 0x0C, set to 0xFFFF by the constructor
    u8 unk10[16]; // 0x10 HYPOTHESIS: sub-object, see the constructor

    // src: the triangle data pointer (the sub-object is copied from src + 0x10).
    hkSphereTriangleAgent(hkContactMgr* contactMgr, void* src);
    virtual ~hkSphereTriangleAgent() {} // inline: the symmetric destructor calls it without a bl

    // Registers the triangle/sphere agent functions with the dispatcher (two pairs, see the .cpp).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Triangle first: the triangle is the first argument (unk0).
    static hkSphereTriangleAgent* createTriangleSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    // Sphere first: the triangle is the second argument (unk1).
    static hkSphereTriangleAgent* createSphereTriangleAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    // The first out-of-line virtual getPenetrations is the key function (vtable in this unit).
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) __attribute__((never_inline));
    // Destructor of the symmetric triangle agent; defined in hkSphereTriangleAgent.cpp.
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
};

// Flipped (symmetric) variant; the class name follows the Havok TU map (see hkSphereBoxAgent.h).
struct hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_ : hkSphereTriangleAgent {
    hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_(hkContactMgr* contactMgr, void* src)
        : hkSphereTriangleAgent(contactMgr, src) {}

    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void updateShapeCollectionFilter() {}
    virtual ~hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_();

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
};
