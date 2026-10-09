#pragma once

#include <havok/hkShape.h>
#include <havok/hkShapeContainer.h>
#include <havok/hkArray.h>
#include <havok/hkWorldCinfo.h>

// MATCH-ONLY-SCOPE: shape collection base (hkShapeCollectionClass.cpp layout). Declared here because no
// header for it exists yet; move it to include/havok/hkShapeCollection.h when its owner adds one.
// Shape collection (0x14 bytes): the embedded container at 0x0C (its vtable is written there by the mesh
// constructors) and disableWelding at 0x10.
struct hkShapeCollection : hkShape {
    hkShapeContainer m_container; // 0x0C HYPOTHESIS: the container interface of the collection
    hkBool m_disableWelding;      // 0x10
};

// One subpart of a mesh (0x30 bytes). Layout from hkMeshShapeClass.cpp.
struct hkMeshShapeSubpart {
    void* m_vertexBase;                 // 0x00
    int m_vertexStriding;               // 0x04
    int m_numVertices;                  // 0x08
    void* m_indexBase;                  // 0x0C
    u8 m_stridingType;                  // 0x10 (IndexStridingType: 1 = int16, 2 = int32)
    u8 m_materialIndexStridingType;     // 0x11 (MaterialIndexStridingType: 1 = int8, 2 = int16)
    int m_indexStriding;                // 0x14
    int m_numTriangles;                 // 0x18
    void* m_materialIndexBase;          // 0x1C
    int m_materialIndexStriding;        // 0x20
    void* m_materialBase;               // 0x24
    int m_materialStriding;             // 0x28
    int m_numMaterials;                 // 0x2C
};

// Triangle mesh shape (0x50 bytes). Layout from hkMeshShapeClass.cpp. The container interface is
// the embedded hkShapeContainer at 0x0C of the collection base.
struct hkMeshShape : hkShapeCollection {
    u8 unk14[0x0C];                         // 0x14 HYPOTHESIS: not in the reflection data
    hkVector4 m_scaling;                    // 0x20
    int m_numBitsForSubpartIndex;           // 0x30
    hkArray<hkMeshShapeSubpart> m_subparts; // 0x34
    hkReal m_radius;                        // 0x40
    int pad[3];                             // 0x44

    hkMeshShape(const hkFinishLoadedObjectFlag& flag); // finish-loading ctor: vtables and refcount only
    virtual ~hkMeshShape();

    static void finishLoadedObjecthkMeshShape(void* p);
    static void cleanupLoadedObjecthkMeshShape(void* p);
    static const void* getVtablehkMeshShape();

    virtual int getType() const; // 0x10
};
