// Havok translation unit hkShape.o (main.dol 0x802D4AB4-0x802D4C7C).
// Functions in address order:
//   0x802D4AB4   456  getMaximumProjection   [map: hkShape__getMaximumProjection]

#include <havok/hkShape.h>

#pragma fp_contract on

// HYPOTHESIS: |half| with the sign bit of direction (bit copy, no fabsf call).
static inline hkReal copySignBits(hkReal mag, hkReal sign) {
    union {
        hkReal f;
        u32 u;
    } m, s;
    m.f = mag;
    s.f = sign;
    m.u ^= s.u & 0x80000000;
    return m.f;
}

hkReal hkShape::getMaximumProjection(const hkVector4& direction) const {
    hkTransform identity;
    identity.m_rotation.m_col0.set(1.0f, 0.0f, 0.0f, 0.0f);
    identity.m_rotation.m_col1.set(0.0f, 1.0f, 0.0f, 0.0f);
    identity.m_rotation.m_col2.set(0.0f, 0.0f, 1.0f, 0.0f);
    identity.m_translation.set(0.0f, 0.0f, 0.0f, 0.0f);

    hkAabb aabb;
    getAabb(identity, 0.0f, aabb);

    hkVector4 center;
    center.set((aabb.m_min.x + aabb.m_max.x) * 0.5f, (aabb.m_min.y + aabb.m_max.y) * 0.5f,
               (aabb.m_min.z + aabb.m_max.z) * 0.5f, 0.0f);
    hkVector4 half;
    half.set((aabb.m_max.x - aabb.m_min.x) * 0.5f, (aabb.m_max.y - aabb.m_min.y) * 0.5f,
             (aabb.m_max.z - aabb.m_min.z) * 0.5f, 0.0f);

    hkVector4 corner;
    corner.set(center.x + copySignBits(half.x, direction.x), center.y + copySignBits(half.y, direction.y),
               center.z + copySignBits(half.z, direction.z), 0.0f);
    return corner.dot3(direction);
}
