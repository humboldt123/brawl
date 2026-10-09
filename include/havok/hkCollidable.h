#pragma once

#include <havok/hkCdBody.h>
#include <havok/hkVector4.h>

// Broad-phase handle (0x04 bytes, base of the typed handle). Layout from hkBroadPhaseHandleClass.cpp.
struct hkBroadPhaseHandle {
    u32 m_id; // 0x00
};

// Typed broad-phase handle (0x0C bytes). Layout from hkTypedBroadPhaseHandleClass.cpp.
struct hkTypedBroadPhaseHandle : hkBroadPhaseHandle {
    s8 m_type;                 // 0x04
    s8 m_ownerOffset;          // 0x05 (offset from this handle to the owning collidable)
    u16 m_objectQualityType;   // 0x06
    u32 m_collisionFilterInfo; // 0x08
};

// Collidable (0x24 bytes). Layout from hkCollidableClass.cpp.
struct hkCollidable : hkCdBody {
    int m_ownerOffset;                          // 0x10 (offset from this collidable to its owner)
    hkTypedBroadPhaseHandle m_broadPhaseHandle; // 0x14
    hkReal m_allowedPenetrationDepth;           // 0x20
};
