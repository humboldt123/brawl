#pragma once

#include <havok/hkShape.h>
#include <havok/hkShapeContainer.h>

// Shape collection (0x14 bytes, hkShapeCollectionClass.cpp layout): the container base at 0x0C (its vtable is
// written there by the mesh constructors) and disableWelding at 0x10. The container sub-vtable sits at 0x28..0x40 of
// the primary vtable, so the collection's first own virtual is slot 0x44 (getCollisionFilterInfo in the hkMeshShape
// map names; hkGroupFilter calls it on the container argument).
struct hkShapeCollection : hkShape, hkShapeContainer {
    HK_DECLARE_REF_ALLOCATOR(0x25) // hkShape's (the container's empty delete would be ambiguous)

    hkBool m_disableWelding; // 0x10

    virtual u32 getCollisionFilterInfo(u32 key) const; // 0x44
};
