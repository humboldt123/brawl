#pragma once

#include <havok/hkBase.h>
#include <havok/hkWorldCinfo.h>
#include <havok/hkShape.h>

struct hkShape;

// Abstract shape container (0x04 bytes, vtable only). Layout from hkShapeContainerClass.cpp.
// Child shapes are addressed by integer keys: getFirstKey (0x10) and getNextKey (0x14) enumerate them, -1 ends.
// Slot order from the hkSingleShapeContainer vtable: 0x08 dtor, 0x0C getNumChildShapes, 0x10 getFirstKey,
// 0x14 getNextKey, 0x18 getChildShape.
struct hkShapeContainer {
    // Scratch space a container may build a child shape in (the shape collection agents keep one on the stack).
    // HYPOTHESIS: the size (the agents' frames leave room for 0x200 bytes). BYTES: the alignment sits on the
    // typedef, which turns copy propagation off in the functions holding one (see hkSimdReal in hkVector4.h).
    typedef char ShapeBuffer[0x200] __attribute__((aligned(16)));

    virtual ~hkShapeContainer() {}
    virtual int getNumChildShapes() const;                                    // 0x0C
    virtual int getFirstKey() const;                                          // 0x10
    virtual int getNextKey(int key) const;                                    // 0x14
    virtual const hkShape* getChildShape(u32 key, ShapeBuffer& buffer) const; // 0x18
};

// Single child container (0x08 bytes). Layout from hkShapeContainerClass.cpp.
struct hkSingleShapeContainer : hkShapeContainer {
    hkShape* m_childShape; // 0x04

    hkSingleShapeContainer(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: only the vtable is written
    virtual ~hkSingleShapeContainer() { if (m_childShape) m_childShape->removeReference(); }

    // Single child keys: one key (0), the child, and no next key. Defined in hkBvShape.cpp.
    virtual int getNumChildShapes() const;                                    // 0x0C
    virtual int getFirstKey() const;                                          // 0x10
    virtual int getNextKey(int key) const;                                    // 0x14
    virtual const hkShape* getChildShape(u32 key, ShapeBuffer& buffer) const; // 0x18

    static void finishLoadedObjecthkSingleShapeContainer(void* p);
    static void cleanupLoadedObjecthkSingleShapeContainer(void* p);
    static const void* getVtablehkSingleShapeContainer();
};
