#pragma once

#include <havok/hkBase.h>
#include <havok/hkWorldCinfo.h>
#include <havok/hkShape.h>

struct hkShape;

// Abstract shape container (0x04 bytes, vtable only). Layout from hkShapeContainerClass.cpp.
// Child shapes are addressed by integer keys: getFirstKey (0x10) and getNextKey (0x14) enumerate them, -1 ends.
struct hkShapeContainer {
    virtual ~hkShapeContainer() {}
    virtual void calcStatistics(hkStatisticsCollector* collector) const; // 0x0C HYPOTHESIS
    virtual int getFirstKey() const;                                     // 0x10 HYPOTHESIS
    virtual int getNextKey(int key) const;                               // 0x14 HYPOTHESIS

    int getNumChildShapes() const;
};

// Single child container (0x08 bytes). Layout from hkShapeContainerClass.cpp.
struct hkSingleShapeContainer : hkShapeContainer {
    hkShape* m_childShape; // 0x04

    hkSingleShapeContainer(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: only the vtable is written
    virtual ~hkSingleShapeContainer() { if (m_childShape) m_childShape->removeReference(); }

    // Single child keys: one key (0), the child, and no next key. Defined in hkBvShape.cpp.
    virtual int getFirstKey() const;     // 0x10
    virtual int getNextKey(int key) const; // 0x14
    int getNumChildShapes() const;
    hkShape* getChildShape() const;

    static void finishLoadedObjecthkSingleShapeContainer(void* p);
    static void cleanupLoadedObjecthkSingleShapeContainer(void* p);
    static const void* getVtablehkSingleShapeContainer();
};
