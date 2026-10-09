#pragma once

#include <havok/hkMeshShape.h>

// Mesh shape with a fast child lookup (0x50 bytes, same layout as hkMeshShape). The second vtable (for the
// container interface at 0x0C) is written by the constructor.
struct hkFastMeshShape : hkMeshShape {
    hkFastMeshShape(const hkFinishLoadedObjectFlag& flag) : hkMeshShape(flag) {} // finish-loading ctor
    virtual ~hkFastMeshShape();

    static void finishLoadedObjecthkFastMeshShape(void* p);
    static void cleanupLoadedObjecthkFastMeshShape(void* p);
    static const void* getVtablehkFastMeshShape();
};
