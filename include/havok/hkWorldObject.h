#pragma once

#include <havok/hkBase.h>
#include <havok/hkWorldCinfo.h> // hkFinishLoadedObjectFlag (HYPOTHESIS: should move to hkBase.h)
#include <havok/hkArray.h>
#include <havok/hkMultiThreadLock.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkProperty.h>
#include <havok/hkShape.h>

struct hkShape;
struct hkWorld;

// Base of all objects that live in an hkWorld (0x58 bytes). Layout from hkWorldObjectClass.cpp.
struct hkWorldObject : hkReferencedObject {
    enum BroadPhaseType {
        BROAD_PHASE_INVALID,
        BROAD_PHASE_ENTITY,
        BROAD_PHASE_PHANTOM,
        BROAD_PHASE_BORDER,
        BROAD_PHASE_MAX_ID,
    };

    hkWorld* m_world;                    // 0x08
    void* m_userData;                    // 0x0C
    const char* m_name;                  // 0x10
    hkMultiThreadLock m_multithreadLock; // 0x14
    hkLinkedCollidable m_collidable;     // 0x1C
    hkArray<hkProperty> m_properties;    // 0x4C (the loaded-object constructor leaves it as loaded)

    hkWorldObject(hkShape* shape, BroadPhaseType type);
    hkWorldObject(hkFinishLoadedObjectFlag flag);
    // Inline: ~hkEntity carries it (vtable store, the shape reference the constructor took, then the members).
    virtual ~hkWorldObject() {
        if (m_collidable.m_shape != 0) {
            m_collidable.m_shape->removeReference();
        }
    }

    void calcStatistics(hkStatisticsCollector* collector) const;
    // Vtable (hkWorldObject's own, in the image): 0x08 dtor, 0x0C calcStatistics, 0x10 setShape. hkRigidBody
    // overrides setShape; entities and phantoms add their own virtuals from 0x14.
    virtual bool setShape(hkShape* shape);
    void addReference();
    void removeReference();
    // HYPOTHESIS: return type and const (map name only)
    const hkCollidable* getCollidable() const;
};
