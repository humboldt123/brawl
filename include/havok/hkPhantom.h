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

// Events passed to the overlap listeners (0x0C bytes): the phantom, the collidable and the listeners' answer.
// HYPOTHESIS: names. On add, a listener sets m_collidableResult to reject the collidable (a word); on remove,
// m_collidableWasAdded tells whether the phantom held it (a byte).
struct hkCollidableAddedEvent {
    hkPhantom* m_phantom;       // 0x00
    hkCollidable* m_collidable; // 0x04
    int m_collidableResult;     // 0x08 0 = accept
};
struct hkCollidableRemovedEvent {
    hkPhantom* m_phantom;       // 0x00
    hkCollidable* m_collidable; // 0x04
    bool m_collidableWasAdded;  // 0x08
};

// Overlap listener interface: the callbacks come first (0x08, 0x0C) and the virtual destructor last (0x10), as in
// hkBroadPhaseBorder's secondary vtable (callback names from its map entries).
struct hkPhantomOverlapListener {
    virtual void collidableAddedCallback(hkCollidableAddedEvent* event) = 0;    // 0x08
    virtual void collidableRemovedCallback(hkCollidableRemovedEvent* event) = 0; // 0x0C
    virtual ~hkPhantomOverlapListener() {}                                    // 0x10
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
    // A new phantom owns its collidable (owner offset back to the phantom).
    hkPhantom(hkShape* shape) : hkWorldObject(shape, BROAD_PHASE_PHANTOM) {
        m_collidable.m_ownerOffset = (u8*)this - (u8*)&m_collidable;
    }

    // Phantoms are freed as class 0x2F (the deleting destructor).
    HK_DECLARE_REF_ALLOCATOR(0x2F)
    static void* operator new(unsigned long, void* p) { return p; } // the loaded-object constructors
    virtual ~hkPhantom();
    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    virtual hkMotion* getMotionState() const = 0;                              // 0x14
    virtual int getType() const = 0;                                           // 0x18
    virtual void calcAabb(hkAabb& out) = 0;                              // 0x1C
    virtual void addOverlappingCollidable(hkCollidable* collidable) = 0;      // 0x20
    virtual hkBool isOverlappingCollidableAdded(hkCollidable* collidable) const = 0; // 0x24
    virtual void removeOverlappingCollidable(hkCollidable* collidable) = 0;   // 0x28
    virtual hkPhantom* clone() const = 0;                                      // 0x2C (map name)
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
// Vtable (lbl_80488290): dtor, calcStatistics, addCollisionPair, removeCollisionPair. HYPOTHESIS: the two pair
// callbacks come from an hkBroadPhaseListener interface that is not modelled separately. Freed as class 0x1f.
struct hkPhantomBroadPhaseListener : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x1f)

    virtual ~hkPhantomBroadPhaseListener() {}
    virtual void addCollisionPair(hkBroadPhaseHandlePair* pair);
    virtual void removeCollisionPair(hkBroadPhaseHandlePair* pair);
};
