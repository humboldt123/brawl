#pragma once

#include <havok/hkMemory.h>

struct hkPenetrationTarget;

// Base of the Havok collision agents. Agents are reference counted (hkReferencedObject) and
// allocated with memory class 0x1d.
//
// Vtable layout, checked against the agent vtables (the hkCollisionAgent vtable itself has zeros in the
// slots that derived agents override):
//   0x08 dtor, 0x0C calcStatistics (hkReferencedObject)
//   0x10 getPenetrations, 0x14 getClosestPoints, 0x18 linearCast, 0x1C processCollision
//   0x20 cleanup, 0x24 updateShapeCollectionFilter, 0x28 invalidateTim, 0x2C warpTime
//   0x30 removePoint, 0x34 commitPotential, 0x38 createZombie
// The parameter lists of the slots are the ones of the agents that name them in their symbols; the
// slot order does not depend on them.
struct hkCollisionAgent : hkReferencedObject {
    int unk8; // 0x08 HYPOTHESIS: per-agent state (contact manager for most agents), zeroed by the constructor

    hkCollisionAgent() {
        unk8 = 0;
    }
    // Used by derived agents that take the per-pair value in their constructor (hkPhantomAgent).
    explicit hkCollisionAgent(int unk8Value) {
        unk8 = unk8Value;
    }
    virtual ~hkCollisionAgent() {}

    // The fourth argument of the query slots is the penetration target (hkPenetrationTarget*); most agent symbols
    // name it void*, so the base uses void* to keep the derived symbols matching.
    virtual void getPenetrations(void* a, void* b, void* c, void* target);   // 0x10
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);  // 0x14
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d); // 0x18
    virtual void processCollision(void* a, void* b, void* c);                                // 0x1C
    virtual void cleanup();                                                                  // 0x20
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);                     // 0x24
    virtual void invalidateTim(void* arg);                                                   // 0x28
    virtual void warpTime(float t0, float t1, void* arg);                                    // 0x2C
    virtual void removePoint(u16 key);                                                       // 0x30
    virtual void commitPotential(u16 key);                                                   // 0x34
    virtual void createZombie(u16 key);                                                      // 0x38
};

// Contact manager that agents store in their 0x08 word (passed as the last argument of the create functions).
// Only the virtual slots used by the sphere agents are declared; names are HYPOTHESIS until the class is recovered.
struct hkContactMgr : hkReferencedObject {
    virtual void unk10(); // 0x10 HYPOTHESIS
    virtual void unk14(); // 0x14 HYPOTHESIS
    virtual void unk18(); // 0x18 HYPOTHESIS: called by hkSphereSphereAgent::cleanup on the 0x08 object
};
