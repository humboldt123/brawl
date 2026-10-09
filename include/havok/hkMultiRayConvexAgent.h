#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

struct hkCollisionDispatcher;

// Agent for a convex shape against a multi-ray shape (shape type HK_SHAPE_MULTI_RAY). Object size 0x20.
// 0x08 (hkCollisionAgent::unk8) is the contact manager, 0x0C points at an array of 16-bit slots and
// 0x10 is the number of slots. cleanup() releases the contact manager once for every slot in use.
struct hkMultiRayConvexAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16* m_slots; // 0x0C
    int m_count;  // 0x10 number of slots
    int unk14;    // 0x14 HYPOTHESIS: capacity/ownership word read by the destructor
    int unk18;    // 0x18
    int unk1C;    // 0x1C

    // HYPOTHESIS: the constructor takes the two bodies of the pair, a third value and the contact manager.
    hkMultiRayConvexAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr);
    virtual ~hkMultiRayConvexAgent();

    // Registers the convex x multi-ray agent functions with the dispatcher (two pairs, see the .cpp).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Creates the convex x multi-ray agent (the convex shape first). Its vtable is the symmetric one.
    static hkMultiRayConvexAgent* createConvexMultiRayAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr);
    // Creates the multi-ray x convex agent in the order given.
    static hkMultiRayConvexAgent* createMultiRayConvexAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void processCollision(void* unk0, void* unk1, void* unk2); // not written yet
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3); // not written yet
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3); // not written yet
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3); // not written yet
};

// HYPOTHESIS: the symmetric (shape-swapped) variant, map name hkSymmetricAgent<hkMultiRayConvexAgent>.
// Its linearCast is not recovered yet.
struct hkSymmetricAgent_21hkMultiRayConvexAgent_ : hkMultiRayConvexAgent {
    virtual ~hkSymmetricAgent_21hkMultiRayConvexAgent_();
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkMultiRayConvexAgent>. The wrappers swap the two shapes and
// replace the collector by a flipping collector on the stack (same scheme as hkSphereBoxAgent).
struct hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_ : hkMultiRayConvexAgent {
    hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_(void* a0, void* a1, void* a2, hkContactMgr* contactMgr)
        : hkMultiRayConvexAgent(a0, a1, a2, contactMgr) {}
    virtual ~hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_();

    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2);

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticLinearCast(void* unk0, void* unk1, void* unk2, void* unk3); // not written yet
};
