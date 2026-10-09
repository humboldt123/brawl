#pragma once

#include <havok/hkDynamicsContactMgr.h>
#include <havok/hkCollidable.h>
#include <havok/hkMemory.h>

struct hkWorld;

// Contact manager that reports contact points to the owners of two collidables (0x18 bytes).
// Layout from the constructor and the factory in hkReportContactMgr.cpp.
struct hkReportContactMgr : hkDynamicsContactMgr {
    HK_DECLARE_REF_ALLOCATOR(0x20)

    hkWorld* m_world;      // 0x08 (copied from the factory)
    const void* m_ownerA; // 0x0C: owner object of the first collidable
    const void* m_ownerB; // 0x10: owner object of the second collidable
    u16 m_maxContacts;    // 0x14: smaller of the two owners' u16 at 0x92 (HYPOTHESIS: contact point limit)

    hkReportContactMgr(hkWorld* world, const void* ownerA, const void* ownerB);
    virtual ~hkReportContactMgr();

    // Factory that creates a hkReportContactMgr for a pair of collidables (0x0C bytes).
    struct Factory : hkReferencedObject {
        hkWorld* m_factoryWorld; // 0x08

        Factory(hkWorld* world);
        virtual hkReportContactMgr* createContactMgr(const hkCollidable& a, const hkCollidable& b,
                                                     const void* input);
    };
};
