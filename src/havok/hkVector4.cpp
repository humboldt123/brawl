// Havok translation unit hkVector4.o (main.dol 0x802878F0-0x80287B04).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802878F0   132  setTransformedPos   [map: hkVector4__setTransformedPos]
//   0x80287974   132  setTransformedInversePos   [map: hkVector4__setTransformedInversePos]
//   0x802879F8   108  setRotatedDir   [map: hkVector4__setRotatedDir]
//   0x80287A64   108  setRotatedInverseDir   [map: hkVector4__setRotatedInverseDir]
//   0x80287AD0    52  __sinit_\hkVector4_cpp   [map: hkVector4cpp____sinit_]
#pragma fp_contract on
#include <havok/hkTransform.h>

// HYPOTHESIS: two static vectors; the second is copied from the first during static initialisation.
static hkVector4 s_zeroVector;

void hkVector4::setTransformedPos(const hkTransform& t, const hkVector4& v) {
    const hkRotation& r = t.m_rotation;
    hkReal vz = v.z;
    hkReal vy = v.y;
    hkReal vx = v.x;
    x = t.m_translation.x + (vx * r.m_col0.x + vy * r.m_col1.x + vz * r.m_col2.x);
    y = t.m_translation.y + (vx * r.m_col0.y + vy * r.m_col1.y + vz * r.m_col2.y);
    z = t.m_translation.z + (vx * r.m_col0.z + vy * r.m_col1.z + vz * r.m_col2.z);
    w = 0.0f;
}

void hkVector4::setTransformedInversePos(const hkTransform& t, const hkVector4& v) {
    const hkRotation& r = t.m_rotation;
    hkReal dx = v.x - t.m_translation.x;
    hkReal dy = v.y - t.m_translation.y;
    hkReal dz = v.z - t.m_translation.z;
    x = dx * r.m_col0.x + dy * r.m_col0.y + dz * r.m_col0.z;
    y = dx * r.m_col1.x + dy * r.m_col1.y + dz * r.m_col1.z;
    z = dx * r.m_col2.x + dy * r.m_col2.y + dz * r.m_col2.z;
    w = 0.0f;
}

void hkVector4::setRotatedDir(const hkRotation& r, const hkVector4& v) {
    hkReal vy = v.y;
    hkReal vx = v.x;
    hkReal vz = v.z;
    x = vx * r.m_col0.x + vy * r.m_col1.x + vz * r.m_col2.x;
    y = vx * r.m_col0.y + vy * r.m_col1.y + vz * r.m_col2.y;
    z = vx * r.m_col0.z + vy * r.m_col1.z + vz * r.m_col2.z;
    w = 0.0f;
}

void hkVector4::setRotatedInverseDir(const hkRotation& r, const hkVector4& v) {
    hkReal vz = v.z;
    hkReal vy = v.y;
    hkReal vx = v.x;
    x = vx * r.m_col0.x + vy * r.m_col0.y + vz * r.m_col0.z;
    y = vx * r.m_col1.x + vy * r.m_col1.y + vz * r.m_col1.z;
    z = vx * r.m_col2.x + vy * r.m_col2.y + vz * r.m_col2.z;
    w = 0.0f;
}
