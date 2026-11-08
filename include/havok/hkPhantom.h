#pragma once

#include <havok/hkWorldObject.h>
#include <havok/hkMemory.h>
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

// Phantom (0x70 bytes). Layout from hkPhantomClass.cpp. Vtable slots from hkPhantom's and hkAabbPhantom's
// vtables in the image (names from the map): 0x08 dtor, 0x0C calcStatistics, 0x10 setShape (hkWorldObject),
// 0x14 getMotionState, 0x18 getType, 0x1C calcAabb, 0x20 addOverlappingCollidable, 0x24
// isOverlappingCollidableAdded, 0x28 removeOverlappingCollidable, 0x2C unknown, 0x30 updateShapeCollectionFilter,
// 0x34 deallocateInternalArrays.
struct hkPhantom : hkWorldObject {
    hkArray<hkPhantomOverlapListener*> m_overlapListeners; // 0x58
    hkArray<hkPhantomListener*> m_phantomListeners;        // 0x64

    hkPhantom(hkFinishLoadedObjectFlag flag) : hkWorldObject(flag) {}

    // Phantoms are freed as class 0x2F (the deleting destructor).
    HK_DECLARE_REF_ALLOCATOR(0x2F)
    virtual ~hkPhantom();
    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    virtual hkMotion* getMotionState() const = 0;                              // 0x14
    virtual int getType() const = 0;                                           // 0x18
    virtual void calcAabb(hkAabb* out) const = 0;                              // 0x1C
    virtual void addOverlappingCollidable(hkCollidable* collidable) = 0;      // 0x20
    virtual hkBool isOverlappingCollidableAdded(hkCollidable* collidable) const = 0; // 0x24
    virtual void removeOverlappingCollidable(hkCollidable* collidable) = 0;   // 0x28
    virtual void unk2C() = 0;                                                  // 0x2C
    virtual void updateShapeCollectionFilter() = 0;                            // 0x30
    virtual void deallocateInternalArrays();                                   // 0x34

    // Called, not inlined, by the destructor that follows it in hkPhantom.o.
    void firePhantomDeleted() __attribute__((never_inline));
    void firePhantomRemoved();
    void firePhantomAdded();
    void addPhantomOverlapListener(hkPhantomOverlapListener* listener);
    void removePhantomOverlapListener(hkPhantomOverlapListener* listener);
    void updateBroadPhase(void* arg);
};

// Broad-phase listener that forwards pair add/remove to the phantom owning each side.
// HYPOTHESIS: base class (hkBroadPhaseListener) not recovered; slots not identified.
struct hkPhantomBroadPhaseListener : hkReferencedObject {
    virtual ~hkPhantomBroadPhaseListener() {}
    void addCollisionPair(hkBroadPhaseHandlePair* pair);
    void removeCollisionPair(hkBroadPhaseHandlePair* pair);
};
