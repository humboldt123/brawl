#pragma once

#include <havok/hkShape.h>
#include <havok/hkShapeContainer.h>

// Shape collection (0x14 bytes, hkShapeCollectionClass.cpp layout): the embedded container at 0x0C (its vtable is
// written there by the mesh constructors) and disableWelding at 0x10.
struct hkShapeCollection : hkShape {
    hkShapeContainer m_container; // 0x0C HYPOTHESIS: the container interface of the collection
    hkBool m_disableWelding;      // 0x10
};
