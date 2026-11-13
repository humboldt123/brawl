#pragma once

#include <havok/hkPhantom.h>
#include <havok/hkAabb.h>
#include <havok/hkRegistry.h>

// Axis-aligned phantom (0x9C bytes). Layout from the constructors and the collidable list in hkAabbPhantom.cpp.
struct hkAabbPhantom : hkPhantom {
    hkAabb m_aabb;                                    // 0x70
    hkArray<hkCollidable*> m_overlappingCollidables;  // 0x90

    // HYPOTHESIS: the second parameter is the collidable's collision filter info.
    hkAabbPhantom(const hkAabb& aabb, u32 collisionFilterInfo = 0) __attribute__((never_inline)); // clone calls it
    hkAabbPhantom(hkFinishLoadedObjectFlag flag);
    virtual ~hkAabbPhantom();

    static void cleanupLoadedObjecthkAabbPhantom(void* p);
    static void finishLoadedObjecthkAabbPhantom(void* p);
    static const void* getVtablehkAabbPhantom() __attribute__((never_inline)); // called by the static initializer
    virtual int getType() const;
    virtual void calcAabb(hkAabb& out);
    virtual void addOverlappingCollidable(hkCollidable* collidable);
    virtual hkBool isOverlappingCollidableAdded(hkCollidable* collidable) const;
    virtual hkPhantom* clone() const;
    virtual void removeOverlappingCollidable(hkCollidable* collidable);
    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    virtual void updateShapeCollectionFilter();
    virtual hkMotion* getMotionState() const;
    void deallocateInternalArrays();
    void setAabb(const hkAabb& aabb);
};

// Static type registration record for phantoms (same layout as hkTypeInfo; see hkMotion.h).
struct hkPhantomTypeInfo : hkTypeInfo {
    hkPhantomTypeInfo(const char* name, void (*finish)(void*), void (*cleanup)(void*), const void* vtable) {
        m_name = name;
        m_finish = finish;
        m_cleanup = cleanup;
        m_vtable = vtable;
    }
};
