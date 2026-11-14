#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkCollidable.h>
#include <havok/hkMemory.h>
#include <havok/hkMultiThreadLock.h>

struct hkAabb;

// Lock object created when a broad phase is shared between threads. It has no state on this platform: the broad
// phase destructor frees one byte (allocator class 0x13).
struct hkCriticalSection {
    HK_DECLARE_SIZED_DELETE(0x13)
    ~hkCriticalSection() {}
};

// Pair of typed broad-phase handles reported by the broad phase (8 bytes).
// HYPOTHESIS: same layout as the pair used by hkPhantom.cpp (which keeps its own copy).
struct hkBroadPhaseHandlePair {
    hkTypedBroadPhaseHandle* m_a; // 0x00
    hkTypedBroadPhaseHandle* m_b; // 0x04
};

// Broad-phase base class. Vtable slots from hk3AxisSweep's vtable (names from the map): 0x08 dtor,
// 0x0C calcStatistics, 0x10 addObject, 0x14 addObjectBatch, 0x18 removeObject, 0x1C removeObjectBatch,
// 0x20 getNumObjects, 0x24/0x28 unknown, 0x2C getAllAabbs, 0x30 getAabb, 0x34 an aabb query (unnamed in the map),
// 0x38 reQuerySingleObject; the later slots are not needed yet. Only the slots something calls are declared
// by name; the others are placeholders (hk3AxisSweep declares its overrides as plain members).
// HYPOTHESIS: the parameter types (the hk3AxisSweep overrides are not written yet; the callers pass the
// handle, the aabb and the pair list in this order).
struct hkBroadPhase : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1f)

    hkBroadPhase();
    ~hkBroadPhase();

    virtual void addObject(hkBroadPhaseHandle* object, const hkAabb& aabb,
                           hkArray<hkBroadPhaseHandlePair>& pairsOut);                    // 0x10
    virtual void addObjectBatch();                                                         // 0x14 (signature unknown)
    virtual void removeObject(hkBroadPhaseHandle* object, hkArray<hkBroadPhaseHandlePair>& pairsOut); // 0x18
    virtual void unk1C();
    virtual void unk20();
    // the batch update of hkSimulation::collideEntitiesBroadPhaseDiscrete (map: hk3AxisSweep::updateAabbs in this
    // slot). HYPOTHESIS: the parameter types beyond the call shape (handles, their aabbs, count, new and old pairs)
    virtual void updateAabbs(hkBroadPhaseHandle** objects, const hkAabb* aabbs, int numObjects,
                             hkArray<hkBroadPhaseHandlePair>& addedPairs,
                             hkArray<hkBroadPhaseHandlePair>& removedPairs); // 0x24
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    // HYPOTHESIS: the name (hkWorld::activateRegion collects the pairs overlapping a box with it)
    virtual void querySingleAabb(const hkAabb& aabb, hkArray<hkBroadPhaseHandlePair>& pairsOut) const; // 0x34

    // HYPOTHESIS: the two empty out-of-line members after the destructor (no caller, no vtable slot).
    void lockImplementation();
    void unlockImplementation();

    hkMultiThreadLock m_multiThreadLock;   // 0x08
    hkCriticalSection* m_criticalSection; // 0x10
};
