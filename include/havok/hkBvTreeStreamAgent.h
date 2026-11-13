#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>
#include <havok/hkVector4.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkBvAgent.h>

struct hkCollisionDispatcher;

struct hkCdBody;
struct hkCollisionInput;
struct hkContactMgr;
struct hkPenetrationTarget;

// Collision agent that streams a bounding volume tree against another shape (0x40 bytes).
// Layout from the constructor and the destructor: the hkCollisionAgent 0x08 word, one word copied
// from the third constructor argument, eight zeroed floats (the cached aabb handed to
// hkBvTreeAgent::calcAabbAndQueryTree), the agent track at 0x30 whose storage is the single inline entry at 0x3C.
struct hkBvTreeStreamAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    int unkC;                      // 0x0C the collision input's dispatcher (copied by the constructor)
    hkReal unk10[8];               // 0x10..0x2F zeroed by the constructor (HYPOTHESIS: two vectors)
    // The agent track of the child agents, managed by hkAgent1nMachine (see cleanup and processCollision).
    hkAgent1nTrack m_list; // 0x30

    hkBvTreeStreamAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                        hkContactMgr* contactMgr);
    virtual ~hkBvTreeStreamAgent();

    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d); // 0x1C
    virtual void cleanup();
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c); // 0x24
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);
    virtual void calcStatistics(hkStatisticsCollector* collector) const;

    static hkBvTreeStreamAgent* createBvTreeShapeAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                       hkCollisionInput& input, hkContactMgr* contactMgr);
    static hkBvTreeStreamAgent* createShapeBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                   hkCollisionInput& input, hkContactMgr* contactMgr);
    static void registerAgent(void* dispatcher);
    static void registerConvexListAgent(void* dispatcher);
    static void registerMultiRayAgent(void* dispatcher);

    // Forwarders to the hkBvTreeAgent static query functions (tail calls in the original).
    void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d);
    void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d);
    void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* d, void* e);
};

// hkSymmetricAgent<hkBvTreeStreamAgent> (map name; created by createBvTreeShapeAgent): the agent with the bodies
// swapped, over hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>. Its deleting destructor tests `this` once per level.
struct hkSymmetricAgent_19hkBvTreeStreamAgent_ : hkSymmetricAgentLinearCast<hkBvTreeStreamAgent> {
    hkSymmetricAgent_19hkBvTreeStreamAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                            hkContactMgr* contactMgr)
        : hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>(bodyA, bodyB, input, contactMgr) {}
    virtual ~hkSymmetricAgent_19hkBvTreeStreamAgent_() {}
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};

// The stream agent registered for MOPP shapes (map name hkMoppBvTreeStreamAgent; its only function is in
// hkBvTreeStreamAgent.o).
struct hkMoppBvTreeStreamAgent : hkBvTreeStreamAgent {
    static void registerAgent(void* dispatcher);
};
