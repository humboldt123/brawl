#pragma once

#include <havok/hkBase.h>
#include <havok/hkCollidable.h>
#include <havok/hkMemory.h>

// Base of the collision filter family (0x18 bytes: hkReferencedObject plus four interface vptrs,
// at offsets 0x08, 0x0C, 0x10 and 0x14). Layout from hkCollisionFilterClass.cpp.
// Each interface vtable is 0x10 bytes (two unused slots, a virtual destructor, one more virtual).
// HYPOTHESIS: the interface names are not recovered yet, so they are numbered. The second virtual
// is a placeholder for the interface's real method, whose name and signature are not known.
struct hkCollisionFilterInterface0 {
    virtual ~hkCollisionFilterInterface0() {}
    virtual void unkVirtual0() {}
};

struct hkCollisionFilterInterface1 {
    virtual ~hkCollisionFilterInterface1() {}
    virtual void unkVirtual1() {}
};

struct hkCollisionFilterInterface2 {
    virtual ~hkCollisionFilterInterface2() {}
    virtual void unkVirtual2() {}
};

// The last interface holds the four isCollisionEnabled overloads of the filters (vtable 0x20 bytes).
struct hkCollisionFilterInterface3 {
    virtual ~hkCollisionFilterInterface3() {}
    virtual void unkVirtual3() {}
    virtual void unkVirtual4() {}
    virtual void unkVirtual5() {}
    virtual void unkVirtual6() {}
    virtual void unkVirtual7() {}
};

struct hkCollisionFilter : hkReferencedObject,
                           hkCollisionFilterInterface0,
                           hkCollisionFilterInterface1,
                           hkCollisionFilterInterface2,
                           hkCollisionFilterInterface3 {
    hkCollisionFilter() {}
    hkCollisionFilter(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkCollisionFilter() {}
};
