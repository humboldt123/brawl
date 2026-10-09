#pragma once

#include <havok/hkBase.h>
#include <havok/hkWorldCinfo.h> // hkFinishLoadedObjectFlag (HYPOTHESIS: should move to hkBase.h)
#include <havok/hkArray.h>
#include <havok/hkMultiThreadLock.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkProperty.h>

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
    hkMultiThreadLock m_multiThreadLock; // 0x14
    hkLinkedCollidable m_collidable;     // 0x1C
    // Typed hkArray<hkProperty> in the original. Untyped here so the finish constructor does not
    // default-construct it (the loaded-object path never writes it).
    hkArrayBase m_properties;            // 0x4C

    hkWorldObject(hkShape* shape);
    hkWorldObject(hkFinishLoadedObjectFlag flag);

    void calcStatistics(hkStatisticsCollector* collector) const;
    bool setShape(hkShape* shape);
    void addReference();
    void removeReference();
};
