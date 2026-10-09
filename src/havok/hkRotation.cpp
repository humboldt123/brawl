// Havok translation unit hkRotation.o (main.dol 0x80285A6C-0x80285CCC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80285A6C   196  set   [map: hkRotation__set]
//   0x80285B30   412  isOrthonormal   [map: hkRotation__isOrthonormal]
#pragma fp_contract on
#include <havok/hkRotation.h>
#include <math.h>

void hkRotation::set(const hkQuaternion& q) {
    hkReal x = q.m_vec.x;
    hkReal y = q.m_vec.y;
    hkReal z = q.m_vec.z;
    hkReal w = q.m_vec.w;
    hkReal x2 = x + x;
    hkReal y2 = y + y;
    hkReal z2 = z + z;
    hkReal yy = y2 * y;
    hkReal zz = z2 * z;
    hkReal xx = x2 * x;
    hkReal xy = y2 * x;
    hkReal zw = z2 * w;
    hkReal xz = z2 * x;
    hkReal yw = y2 * w;
    hkReal yz = z2 * y;
    hkReal xw = x2 * w;

    m_col0.x = 1.0f - (yy + zz);
    m_col0.y = xy + zw;
    m_col0.z = xz - yw;
    m_col0.w = 0.0f;
    m_col1.x = xy - zw;
    m_col1.y = 1.0f - (xx + zz);
    m_col1.z = yz + xw;
    m_col1.w = 0.0f;
    m_col2.x = xz + yw;
    m_col2.y = yz - xw;
    m_col2.z = 1.0f - (xx + yy);
    m_col2.w = 0.0f;
}

bool hkRotation::isOrthonormal(hkReal tolerance) const {
    const hkReal* m = &m_col0.x;
    if (!((hkReal)fabs((m[2] * m[2] + m[0] * m[0] + m[1] * m[1]) - 1.0f) <= tolerance)) {
        return false;
    }
    if (!((hkReal)fabs((m[6] * m[6] + m[4] * m[4] + m[5] * m[5]) - 1.0f) <= tolerance)) {
        return false;
    }
    if (!((hkReal)fabs((m[10] * m[10] + m[8] * m[8] + m[9] * m[9]) - 1.0f) <= tolerance)) {
        return false;
    }
    if (tolerance < (hkReal)fabs((m[0] * m[5] - m[1] * m[4]) - m[10])) {
        return false;
    }
    if (tolerance < (hkReal)fabs((m[1] * m[6] - m[2] * m[5]) - m[8])) {
        return false;
    }
    if (tolerance < (hkReal)fabs((m[2] * m[4] - m[0] * m[6]) - m[9])) {
        return false;
    }
    return true;
}
