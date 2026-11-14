#pragma once

#include <havok/hkBvTreeShape.h>
#include <havok/hkArray.h>

// Compiled MOPP tree of a shape collection. HYPOTHESIS: m_info (the code's offset and scale); the byte code size
// at 0x24 is what hkMoppAgent::createBvBvAgent compares.
struct hkMoppCode : hkReferencedObject {
    hkVector4 m_info;   // 0x10
    hkArray<u8> m_data; // 0x20
};

// BV tree shape whose tree is a MOPP code (0x14).
struct hkMoppBvTreeShape : hkBvTreeShape {
    const hkMoppCode* m_code; // 0x14

    const hkMoppCode* getMoppCode() const { return m_code; }
};
