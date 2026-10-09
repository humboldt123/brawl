#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkPhantomAgent.h>

struct hkCollisionDispatcher;

// Bounding-volume collision agent. It owns up to two sub-agents: A at 0x0C (always set by the
// constructor) and B at 0x10 (null after construction, optional).
//
// HYPOTHESIS: this class derives from hkReferencedObject rather than hkCollisionAgent. The
// vtable slots that hkCollisionAgent declares (invalidateTim at 0x10, ...) do not match the
// slots the sub-agent calls in this unit use (cleanup 0x20, invalidateTim 0x28, warpTime 0x2C,
// removePoint 0x30, commitPotential 0x34, createZombie 0x38), so the virtual list is spelled out
// here in that order. Sub-agents are typed as hkBvAgent since all agents share that layout.
// The slots 0x10..0x24 are a hypothesis: the names come from the map, the order does not.
struct hkBvAgent : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkContactMgr* m_contactMgr; // 0x08 passed to the constructor as its last argument
    hkBvAgent* m_childA;        // 0x0C
    hkBvAgent* m_childB;        // 0x10

    // HYPOTHESIS: the constructor takes the pair as passed by the dispatcher (createShapeBvAgent
    // swaps the first two). Parameter types are not recovered yet.
    hkBvAgent(void* a1, void* a2, void* a3, hkContactMgr* contactMgr);
    virtual ~hkBvAgent() {}

    // Creates the agent for the pair in the order given (create function of the dispatcher table).
    static hkBvAgent* createBvShapeAgent(void* a1, void* a2, void* a3, hkContactMgr* contactMgr);
    // Same, with the first two arguments swapped; the object has the hkSymmetricBvAgent vtable.
    static hkBvAgent* createShapeBvAgent(void* a1, void* a2, void* a3, hkContactMgr* contactMgr);

    virtual void processCollision();                                      // 0x10 HYPOTHESIS
    virtual void getClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target); // 0x14 HYPOTHESIS
    virtual void linearCast();                                            // 0x18 HYPOTHESIS
    virtual void getPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target); // 0x1C HYPOTHESIS
    virtual void cleanup();                                               // 0x20
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);  // 0x24 HYPOTHESIS
    virtual void invalidateTim(void* arg);                                // 0x28
    virtual void warpTime(float t0, float t1, void* arg);                 // 0x2C
    virtual void removePoint(void* arg);                                  // 0x30
    virtual void commitPotential(void* arg);                              // 0x34
    virtual void createZombie(void* arg);                                 // 0x38

    static void registerAgent(void* dispatcher);
    static void staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target);
};

// HYPOTHESIS: the symmetric variant (map name hkSymmetricAgent<hkBvAgent>). It differs from
// hkBvAgent by its vtable only; the constructor is the base constructor followed by the vptr store.
struct hkSymmetricBvAgent : hkBvAgent {
    hkSymmetricBvAgent(void* a1, void* a2, void* a3, hkContactMgr* contactMgr)
        : hkBvAgent(a1, a2, a3, contactMgr) {}
};

// HYPOTHESIS: penetration target wrapper built on the stack by hkSymmetricAgentLinearCast. Holds the
// caller's target and a flag that is always false here.
struct hkSymmetricTarget : hkPenetrationTarget {
    hkSymmetricTarget(hkPenetrationTarget* target) : m_flag(0), m_target(target) {}
    virtual void unkSlot08();
    virtual void forwardPenetrations(void* a, void* b, void* c);

    u8 m_flag;                    // 0x04 relative to the object
    hkPenetrationTarget* m_target; // 0x08
};

// HYPOTHESIS: template of the map names hkSymmetricAgentLinearCast<T>::*, instantiated for hkBvAgent.
// Each wrapper swaps the first two pair arguments and calls the T version with a hkSymmetricTarget
// around the target.
template <class T>
struct hkSymmetricAgentLinearCast : T {
    virtual void getPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target);
    virtual void getClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target);
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);
};
