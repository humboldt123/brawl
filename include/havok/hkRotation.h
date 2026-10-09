#pragma once

#include <havok/hkQuaternion.h>

// 3x3 rotation matrix stored as three column vectors (column-major).
struct hkRotation {
    hkVector4 m_col0; // 0x00
    hkVector4 m_col1; // 0x10
    hkVector4 m_col2; // 0x20

    void set(const hkQuaternion& q);
    // tolerance is passed in f1 (HYPOTHESIS: a caller-supplied epsilon).
    bool isOrthonormal(hkReal tolerance) const;
};
