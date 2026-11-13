#pragma once

#include <havok/hkMemory.h>
#include <havok/hkCollisionInput.h>

struct hkCdBody;
struct hkPenetrationTarget;
struct hkCollidable;
struct hkProcessCollisionOutput;

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

    // Agents are freed as class 0x1D (also the weak hkCollisionAgent deleting destructors).
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkCollisionAgent() {
        unk8 = 0;
    }
    // Used by derived agents that take the per-pair value in their constructor (hkPhantomAgent).
    explicit hkCollisionAgent(int unk8Value) {
        unk8 = unk8Value;
    }
    virtual ~hkCollisionAgent() {}

    // BYTES: the bodies are const references and the inputs plain references. MWCC lets a load through a
    // pointer-to-const parameter move past stores to stack objects (the child bodies the agents build), and the agents'
    // body reads are scheduled that way; their input reads are not (the symmetric linear casts copy the input in strict
    // load/store order, the dispatcher and filter loads stay behind the child-body stores), which only a non-const
    // input parameter gives. The same holds for the agent constructors and create functions. The collector / output
    // arguments are still void* (their classes are not modeled everywhere yet).
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);   // 0x10
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);  // 0x14
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d); // 0x18
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);                                // 0x1C
    virtual void cleanup();                                                                  // 0x20
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);                     // 0x24
    virtual void invalidateTim(void* arg);                                                   // 0x28
    virtual void warpTime(float t0, float t1, void* arg);                                    // 0x2C
    virtual void removePoint(u16 key);                                                       // 0x30
    virtual void commitPotential(u16 key);                                                   // 0x34
    virtual void createZombie(u16 key);                                                      // 0x38
};

// Contact manager that agents store in their 0x08 word (passed as the last argument of the create functions).
// Slot names from hkNullContactMgr's vtable in the image (lbl_80488348: 0x10 addContactPoint, 0x14
// reserveContactPoints, 0x18 removeContactPoint, 0x1C processContact, 0x20 cleanup, 0x24 addToi, 0x28 removeToi,
// 0x2C processToi). HYPOTHESIS: the parameters of the slots nothing here calls yet (declared without any).
struct hkContactPoint;
struct hkContactPointProperties;
struct hkContactPointMaterial;
struct hkGskCache;
struct hkCollisionConstraintOwner;

struct hkContactMgr : hkReferencedObject {
    // 0x10 HYPOTHESIS: the parameter list (hkSimpleConstraintContactMgr reads the two bodies and the point).
    virtual u16 addContactPoint(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& input,
                                hkProcessCollisionOutput& output, const hkGskCache* cache, hkContactPoint& cp);
    // 0x14: returns 1 (failure) when the points do not fit.
    virtual hkResult reserveContactPoints(int numPoints);
    // 0x18 HYPOTHESIS: Havok's hkContactMgr::removeContactPoint(hkContactPointId), the id being 16 bits; the agents' cleanup
    // calls it once for every contact point they still hold (key != 0xFFFF).
    virtual void removeContactPoint(u16 contactPointId);
    // 0x1C: hands an agent's new contact points to the manager (hkSimulation::processAgentCollideDiscrete calls it
    // with the entry's two collidables). HYPOTHESIS: the parameter types beyond the call shape.
    virtual void processContact(const hkCollidable& a, const hkCollidable& b, const hkProcessCollisionInput& input,
                                hkProcessCollisionOutput& output);
    // 0x20: hkWorldAgentUtil::removeAgent calls it after the agent entry is destroyed.
    virtual void cleanup();
    // 0x24..0x2C time-of-impact events. HYPOTHESIS: the parameter lists (register use of
    // hkSimpleConstraintContactMgr's overrides).
    virtual int addToi(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& input,
                       hkProcessCollisionOutput& output, hkReal toi, hkContactPoint& cp, hkReal& projectedVelocity,
                       hkContactPointMaterial& materialOut);
    virtual void removeToi(const hkCollidable& a, const hkCollidable& b, hkContactPointMaterial& material);
    // HYPOTHESIS: the event is the continuous simulation's toi record (entities at 0x04/0x08, separating velocity at
    // 0x0C, contact point at 0x10, properties at 0x20); the other two arguments go to the simple collision response.
    virtual void processToi(const void* toiEvent, hkReal rotateNormal, void* entitiesOut);
};

// Makes the contact manager of a new agent (hkCollisionDispatcher holds one per pair of material response types).
// HYPOTHESIS: the parameter types (hkWorldAgentUtil::addAgent passes the two collidables and the input).
struct hkContactMgrFactory : hkReferencedObject {
    virtual hkContactMgr* createContactMgr(const hkCollidable& a, const hkCollidable& b,
                                           const hkCollisionInput& input); // 0x10
};
