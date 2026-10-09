#pragma once

#include <havok/hkPhantom.h>
#include <havok/hkAabb.h>

// Axis-aligned phantom (0x9C bytes). Layout from the constructors and the collidable list in hkAabbPhantom.cpp.
struct hkAabbPhantom : hkPhantom {
    hkAabb m_aabb;                                    // 0x70
    hkArray<hkCollidable*> m_overlappingCollidables;  // 0x90

    hkAabbPhantom(hkFinishLoadedObjectFlag flag);
    virtual ~hkAabbPhantom();

    void cleanupLoadedObject();
    static void* getVtable();
    virtual int getType() const;
    virtual void calcAabb(hkAabb* out) const;
    virtual void addOverlappingCollidable(hkCollidable* collidable);
    virtual hkBool isOverlappingCollidableAdded(hkCollidable* collidable) const;
    virtual void removeOverlappingCollidable(hkCollidable* collidable);
    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    virtual void updateShapeCollectionFilter();
    virtual hkMotion* getMotionState() const;
    void deallocateInternalArrays();
};
