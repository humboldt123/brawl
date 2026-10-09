#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

// HYPOTHESIS: the child agent stored at 0x0C of hkTransformAgent. The transform agent forwards its
// hooks to this child. Slots 0x08..0x1C are not identified yet (the first slot is the destructor).
struct hkTransformChildAgent {
    virtual void unkSlot08() = 0;
    virtual void unkSlot0C() = 0;
    virtual void unkSlot10() = 0;
    virtual void unkSlot14() = 0;
    virtual void unkSlot18() = 0;
    virtual void unkSlot1C() = 0;
    virtual void cleanupChild() = 0;                                  // 0x20 HYPOTHESIS
    virtual void updateShapeCollectionFilterChild(void* a, void* b, void* c) = 0; // 0x24
    virtual void invalidateTimChild(void* arg) = 0;                                  // 0x28
    virtual void warpTimeChild(float t0, float t1, void* arg) = 0;                   // 0x2C
    virtual void removePointChild(u16 key) = 0;                                      // 0x30
    virtual void commitPotentialChild(u16 key) = 0;                                  // 0x34
    virtual void createZombieChild(u16 key) = 0;                                     // 0x38
};

// Transform agent: wraps a child agent (0x0C) and forwards the collision hooks to it. Object size 0x10.
struct hkCollisionDispatcher;

struct hkTransformAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkTransformChildAgent* m_childAgent; // 0x0C

    // Registers the transform agent pairs (shape type 0x19) with the dispatcher.
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Create functions (not written yet). HYPOTHESIS: the parameters follow the dispatcher create signature.
    static hkTransformAgent* createTransformAAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    static hkTransformAgent* createTransformBAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual ~hkTransformAgent();

    virtual void cleanup();
    // Overrides of the hkCollisionAgent slots with the base parameter lists.
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);
    virtual void removePoint(u16 key);
    virtual void commitPotential(u16 key);
    virtual void createZombie(u16 key);

    // Not written yet (see hkTransformAgent.cpp): linear cast, closest points, penetrations, processCollision.
    virtual void processCollision(void* unk0, void* unk1, void* unk2);
    virtual void linearCast(void* unk0, void* unk1, void* unk2, void* unk3, void* unk4);
    static void staticLinearCast(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2);
};

// HYPOTHESIS: the symmetric (shape-swapped) variant hkSymmetricAgent<hkTransformAgent> (map name
// hkSymmetricAgent_16hkTransformAgent_). Only its destructor is written; its linearCast is not recovered.
struct hkSymmetricAgent_16hkTransformAgent_ : hkTransformAgent {
    virtual ~hkSymmetricAgent_16hkTransformAgent_();
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkTransformAgent>. The wrappers swap the two shapes and
// replace the collector by a flipping collector on the stack (same scheme as hkSphereBoxAgent).
struct hkSymmetricAgentLinearCast_16hkTransformAgent_ : hkSymmetricAgent_16hkTransformAgent_ {
    virtual ~hkSymmetricAgentLinearCast_16hkTransformAgent_();

    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2);

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticLinearCast(void* unk0, void* unk1, void* unk2, void* unk3); // not written yet
};
