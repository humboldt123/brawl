#pragma once

#include <havok/hkBase.h>
#include <havok/hkMemory.h>
#include <havok/hkWorld.h>
#include <havok/hkPhantom.h>

struct hkEntity;

// Six phantoms around the edge of the world broadphase: a body that overlaps one of them is handled according to
// m_type (map names hkBroadPhaseBorder__*). Object size 0x30, freed as class 0x0C. The class's own virtuals follow the
// two secondary vtables (slots 0x34..0x44 of the combined vtable).
struct hkBroadPhaseBorder : hkReferencedObject, hkWorldDeletionListener, hkPhantomOverlapListener {
    HK_DECLARE_REF_ALLOCATOR(0x0c)

    // What happens to a body that leaves the broadphase. HYPOTHESIS: names (Havok's hkBroadPhaseBorder::Type); this
    // build fixes the body for both of the first two values.
    enum Type {
        BROADPHASE_BORDER_ASSERT = 0,
        BROADPHASE_BORDER_FIX_ENTITY = 1,
        BROADPHASE_BORDER_REMOVE_ENTITY = 2,
        BROADPHASE_BORDER_DO_NOTHING = 3
    };

    hkWorld* m_world;          // 0x10
    hkPhantom* m_phantoms[6];  // 0x14
    int m_type;                // 0x2C

    // Puts a flat phantom on each of the six faces of the world's broadphase extents and adds them to the world.
    hkBroadPhaseBorder(hkWorld* world, Type type);
    virtual ~hkBroadPhaseBorder();

    virtual void maxPositionExceededCallback(hkEntity* entity);            // 0x34
    // Removes the phantoms from the world and stops listening (called with the world still set).
    virtual void deactivate();                                             // 0x38
    virtual void collidableAddedCallback(hkCollidableAddedEvent* event);    // 0x3C
    virtual void collidableRemovedCallback(hkCollidableRemovedEvent* event);  // 0x40
    virtual void worldDeletedCallback(hkWorld* world);                     // 0x44
};
