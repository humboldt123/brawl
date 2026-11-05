#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>

struct hkCollisionDispatcher;
struct hkCdBody;
struct hkCollisionInput;

// One child of the collection with the agent that handles it (0x08 bytes).
struct hkShapeCollectionAgentKeyAgentPair {
    u32 m_key;                 // 0x00 shape key of the child
    hkCollisionAgent* m_agent; // 0x04
};

// Collision agent for a shape collection against another shape (0x38 bytes): one child agent per child shape of
// the collection (body A) that the input's filter lets collide with body B. The agent slots take
// (collection body, other body, hkCollisionInput, target/result) like every agent.
struct hkShapeCollectionAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for the factories

    hkInplaceArray<hkShapeCollectionAgentKeyAgentPair, 4> m_agents; // 0x0C (storage for four children at 0x18)

    // Arguments: collection body, other body, collision input, contact manager (kept in the 0x08 word).
    hkShapeCollectionAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, hkContactMgr* mgr);
    virtual ~hkShapeCollectionAgent();

    static hkShapeCollectionAgent* createListAAgent(void* a, void* b, void* c, int d);
    static hkShapeCollectionAgent* createListBAgent(void* a, void* b, void* c, int d);
    static void registerAgent(void* dispatcher);

    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);
    virtual void processCollision(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void cleanup();
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);

    // Same queries without an agent: each child goes to the dispatcher's static function for its shape type pair.
    static void staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                      void* target);
    static void staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                       void* target);
    static void staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                 void* target, void* d);
};



// HYPOTHESIS: agent variant created by createListBAgent. Its vtable (lbl_80486F38) is not identified
// yet; the class adds no members and only replaces the vtable after the base constructor.
struct hkShapeCollectionAgentVariant : hkShapeCollectionAgent {
    hkShapeCollectionAgentVariant(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                  hkContactMgr* mgr)
        : hkShapeCollectionAgent(bodyA, bodyB, input, mgr) {}
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);
};
