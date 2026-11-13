#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkPhantomAgent.h>

struct hkCollisionDispatcher;
struct hkCdBody;
struct hkCollisionInput;
struct hkContactMgr;

// Bounding-volume collision agent. It owns up to two sub-agents: A at 0x0C (always set by the
// constructor) and B at 0x10 (null after construction, optional).
//
// Derives from hkCollisionAgent: the vtable matches the base slot order (checked against the
// lbl_804867BC vtable). Sub-agents are typed as hkBvAgent since all agents share that layout.
// The slots 0x10..0x1C are a hypothesis: the names come from the map, the order does not.
struct hkBvAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    // 0x08 (hkCollisionAgent::unk8) holds the contact manager passed to the constructor as its last argument.
    hkCollisionAgent* m_childA; // 0x0C agent of the bounding volume shape against B
    hkCollisionAgent* m_childB; // 0x10 agent of the child shape against B, made on the first overlap

    // The constructor takes the pair as passed by the dispatcher (createShapeBvAgent swaps the first two).
    hkBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input, hkContactMgr* contactMgr);
    virtual ~hkBvAgent() {}

    // Creates the agent for the pair in the order given (create function of the dispatcher table).
    static hkBvAgent* createBvShapeAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input, hkContactMgr* contactMgr);
    // Same, with the first two arguments swapped; the object has the hkSymmetricBvAgent vtable.
    static hkBvAgent* createShapeBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input, hkContactMgr* contactMgr);

    // Overrides of the hkCollisionAgent slots, with the base parameter lists.
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);   // 0x10 HYPOTHESIS
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);  // 0x14 HYPOTHESIS
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d); // 0x18 HYPOTHESIS
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);                // 0x1C HYPOTHESIS
    virtual void cleanup();                                                  // 0x20
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);     // 0x24 HYPOTHESIS
    virtual void invalidateTim(void* arg);                                   // 0x28
    virtual void warpTime(float t0, float t1, void* arg);                    // 0x2C
    virtual void removePoint(u16 key);                                       // 0x30
    virtual void commitPotential(u16 key);                                   // 0x34
    virtual void createZombie(u16 key);                                      // 0x38

    static void registerAgent(void* dispatcher);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};

// HYPOTHESIS: the symmetric variant (map name hkSymmetricAgent<hkBvAgent>). It differs from
// hkBvAgent by its vtable only; the constructor is the base constructor followed by the vptr store.
struct hkSymmetricBvAgent : hkBvAgent {
    hkSymmetricBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input, hkContactMgr* contactMgr)
        : hkBvAgent(bodyA, bodyB, input, contactMgr) {}
    virtual ~hkSymmetricBvAgent() {}
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};

// HYPOTHESIS: template of the map names hkSymmetricAgentLinearCast<T>::*, instantiated for hkBvAgent.
// Each wrapper swaps the first two pair arguments and calls the T version with a flip collector
// around the target.
template <class T>
struct hkSymmetricAgentLinearCast : T {
    // Forwards to T's constructor. A member template, so the explicit instantiations of the class
    // (hkBvAgent.cpp) do not instantiate it for agents without a four-argument constructor.
    template <class A, class B, class C, class D>
    hkSymmetricAgentLinearCast(A& a, B& b, C& c, D d) : T(a, b, c, d) {}

    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, hkPenetrationTarget* target);
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);
};
