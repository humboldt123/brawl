#pragma once

#include <havok/hkIterativeLinearCastAgent.h>
#include <havok/hkArray.h>

struct hkCollisionDispatcher;
struct hkCdBody;
struct hkCollisionInput;

// One child entry of the multi-sphere agent: a 32-bit key and the child agent (8 bytes per entry).
struct hkMultiSphereAgentEntry {
    u32 unk0; // 0x00 HYPOTHESIS: key of the child pair
    hkCollisionAgent* agent; // 0x04 child agent (cleanup is called on it)
};

// Multi-sphere agent: a list of child agents. Object size 0x38.
struct hkMultiSphereAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkInplaceArray<hkMultiSphereAgentEntry, 4> m_entries; // 0x0C (inline storage for four entries at 0x18)

    // One child agent per sphere of A's multi-sphere shape (unk0, unk1, unk2: bodyA, bodyB, input).
    hkMultiSphereAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input, hkContactMgr* contactMgr);
    static void* operator new(unsigned long, void* p) { return p; }
    virtual ~hkMultiSphereAgent() {}

    // Registers the two list agents (multi-sphere against all shape types, both orders).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static hkMultiSphereAgent* createListAAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    static hkMultiSphereAgent* createListBAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                      void* target);
    static void staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                       void* target);
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
    static void staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                 void* target, void* d);
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d); // not written yet
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkMultiSphereAgent> (map name): the flipped queries. The symmetric agent
// derives from it (its vtable 0x80486DB0 takes these overrides and its destructor tests three levels of `this`).
struct hkSymmetricAgentLinearCast_18hkMultiSphereAgent_ : hkMultiSphereAgent {
    hkSymmetricAgentLinearCast_18hkMultiSphereAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                     hkCollisionInput& input, hkContactMgr* contactMgr)
        : hkMultiSphereAgent(bodyA, bodyB, input, contactMgr) {}
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);
};

// Flipped (symmetric) variant created by createListBAgent; the class name follows the Havok TU map.
struct hkSymmetricAgent_18hkMultiSphereAgent_ : hkSymmetricAgentLinearCast_18hkMultiSphereAgent_ {
    hkSymmetricAgent_18hkMultiSphereAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                           hkContactMgr* contactMgr)
        : hkSymmetricAgentLinearCast_18hkMultiSphereAgent_(bodyA, bodyB, input, contactMgr) {}
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};

// Multi-sphere against triangle agent (object size 0x2C, map names; in hkMultiSphereAgent.o). The 0x08 word is the
// contact manager, 0x0C the triangle's closest-point cache (hkCollideTriangleUtil::setupClosestPointTriangleCache) and
// 0x1C one contact point id per sphere (0xFFFF when unused, released by cleanup). Its static linear cast goes to the
// iterative linear cast.
struct hkMultiSphereTriangleAgent : hkIterativeLinearCastAgent {
    u8 m_closestPointTriangleCache[16]; // 0x0C
    u16 m_contactPointId[8];            // 0x1C HYPOTHESIS: eight = the most spheres a multi-sphere shape holds

    // triangleBody: the body holding the triangle (bodyB, or bodyA for the symmetric variant).
    hkMultiSphereTriangleAgent(const hkCdBody& triangleBody, hkContactMgr* contactMgr);
    virtual ~hkMultiSphereTriangleAgent() {}

    // Registers the triangle/multi-sphere agent functions with the dispatcher (both orders).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Triangle first: the triangle is bodyA; builds the symmetric variant.
    static hkMultiSphereTriangleAgent* createTriangleMultiSphereAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                                      hkCollisionInput& input, hkContactMgr* contactMgr);
    // Multi-sphere first: the triangle is bodyB; builds the plain agent.
    static hkMultiSphereTriangleAgent* createMultiSphereTriangleAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                                      hkCollisionInput& input, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target); // not written yet
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target); // not written yet
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target); // not written yet
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target); // not written yet
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkMultiSphereTriangleAgent> (map name).
struct hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_ : hkMultiSphereTriangleAgent {
    hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_(const hkCdBody& triangleBody, hkContactMgr* contactMgr)
        : hkMultiSphereTriangleAgent(triangleBody, contactMgr) {}
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);
};
