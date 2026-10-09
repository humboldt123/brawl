#pragma once

#include <havok/hkWorldObject.h>
#include <havok/hkArray.h>
#include <havok/hkAabb.h>

struct hkPhantom;
struct hkCollidable;
struct hkMotion;
struct hkStatisticsCollector;
struct hkBroadPhaseHandlePair;

// Callback interface for phantom add/remove/delete notifications. Vtable slots (byte offset):
// 0x08 dtor, 0x0C phantomAdded, 0x10 phantomRemoved, 0x14 unknown, 0x18 phantomDeleted.
// HYPOTHESIS: slot names come from the firing functions in hkPhantom.cpp, not from the vtable itself.
struct hkPhantomListener {
    virtual ~hkPhantomListener() {}
    virtual void phantomAdded(hkPhantom* phantom) = 0;     // 0x0C
    virtual void phantomRemoved(hkPhantom* phantom) = 0;   // 0x10
    virtual void unk14(hkPhantom* phantom) = 0;            // 0x14
    virtual void phantomDeleted(hkPhantom* phantom) = 0;   // 0x18
};

// Event passed to overlap listeners (0x0C bytes). HYPOTHESIS: field names.
// m_handled is set by the listener to veto the change (add) or is preset to "already present" (remove).
struct hkPhantomOverlapEvent {
    hkPhantom* m_phantom;       // 0x00
    hkCollidable* m_collidable; // 0x04
    bool m_handled;             // 0x08
};

// Overlap listener interface (no virtual dtor: first virtual is at 0x08). HYPOTHESIS: names.
struct hkPhantomOverlapListener {
    virtual void overlapAdded(hkPhantomOverlapEvent* event) = 0;   // 0x08
    virtual void overlapRemoved(hkPhantomOverlapEvent* event) = 0; // 0x0C
};

// Phantom (0x70 bytes). Layout from hkPhantomClass.cpp. Vtable slots (byte offset) known so far:
// 0x08 dtor, 0x0C calcStatistics, 0x10 getType (HYPOTHESIS, as in hkShape), 0x14 calcAabb (HYPOTHESIS),
// 0x20 addOverlappingCollidable, 0x24 isOverlappingCollidableAdded (HYPOTHESIS), 0x28 removeOverlappingCollidable.
// Slots 0x18 and 0x1C are not identified yet.
struct hkPhantom : hkWorldObject {
    hkArray<hkPhantomOverlapListener*> m_overlapListeners; // 0x58
    hkArray<hkPhantomListener*> m_phantomListeners;        // 0x64

    hkPhantom(hkFinishLoadedObjectFlag flag) : hkWorldObject(flag) {}

    virtual ~hkPhantom();
    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    virtual int getType() const = 0;
    virtual void calcAabb(hkAabb* out) const = 0;
    virtual void unk18() = 0;
    virtual void updateShapeCollectionFilter() = 0;
    virtual void addOverlappingCollidable(hkCollidable* collidable) = 0;      // 0x20
    virtual hkBool isOverlappingCollidableAdded(hkCollidable* collidable) const = 0; // 0x24 HYPOTHESIS
    virtual void removeOverlappingCollidable(hkCollidable* collidable) = 0;   // 0x28
    virtual hkMotion* getMotionState() const = 0;

    void firePhantomDeleted();
    void firePhantomRemoved();
    void firePhantomAdded();
    void addPhantomOverlapListener(hkPhantomOverlapListener* listener);
    void removePhantomOverlapListener(hkPhantomOverlapListener* listener);
    void deallocateInternalArrays();
    void updateBroadPhase(void* arg);
};

// Broad-phase listener that forwards pair add/remove to the phantom owning each side.
// HYPOTHESIS: base class (hkBroadPhaseListener) not recovered; slots not identified.
struct hkPhantomBroadPhaseListener : hkReferencedObject {
    virtual ~hkPhantomBroadPhaseListener() {}
    void addCollisionPair(hkBroadPhaseHandlePair* pair);
    void removeCollisionPair(hkBroadPhaseHandlePair* pair);
};
