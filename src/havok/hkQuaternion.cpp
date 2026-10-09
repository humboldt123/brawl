// Havok translation unit hkQuaternion.o (main.dol 0x8028564C-0x80285A6C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8028564C   152  setAxisAngle   [map: hkQuaternion__setAxisAngle]
//   0x802856E4   552  quaternionFromRotatation   [map: hkQuadReal__quaternionFromRotatation]
//   0x8028590C     4  set   [map: hkQuaternion__set]
//   0x80285910   312  setFlippedRotation   [map: hkQuaternion__setFlippedRotation]
#pragma fp_contract on
#include <havok/hkQuaternion.h>
#include <havok/hkRotation.h>
#include <math.h>

// HYPOTHESIS: identity quaternion held in a static (static initialisation in __sinit_).
static hkQuaternion s_identityQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

// IEEE single-precision +infinity (0x7F800000), used when the square root argument is not positive.
static hkReal hkInfinity() {
    union {
        u32 i;
        hkReal f;
    } u;
    u.i = 0x7f800000;
    return u.f;
}

// Quaternion from axis (unit vector) and angle: (sin(a/2) * axis, cos(a/2)).
void hkQuaternion::setAxisAngle(const hkVector4& axis, hkReal angle) {
    hkReal half = 0.5f * angle;
    hkReal s = (hkReal)sin(half);
    m_vec.x = s * axis.x;
    m_vec.y = s * axis.y;
    m_vec.z = s * axis.z;
    m_vec.w = s * axis.w;
    m_vec.w = (hkReal)cos(half);
}

// 1/sqrt(x) with the hardware estimate and one Newton step; +infinity for non-positive x.
static hkReal hkRsqrtApprox(hkReal x) {
    if (0.0f < x) {
        hkReal r = (hkReal)__frsqrte(x);
        return (0.5f * r) * -(r * (x * r) - 3.0f);
    }
    return hkInfinity();
}

// Shoemake-style quaternion from a rotation matrix (column-major m[col * 4 + row]).
void hkQuadReal::quaternionFromRotatation(hkQuaternion* out, const hkRotation& rot) {
    const hkReal* m = &rot.m_col0.x;
    hkVector4 tmpVec;
    hkReal* tmp = &tmpVec.x;
    hkReal trace = (m[0] + m[5]) + m[10];
    hkReal f1;
    hkReal f2;

    if (0.0f < trace) {
        f1 = hkRsqrtApprox(trace + 1.0f);
        f2 = 0.5f / (1.0f / f1);
        tmp[1] = f2 * (m[8] - m[2]);
        tmp[0] = f2 * (m[6] - m[9]);
        tmp[2] = f2 * (m[1] - m[4]);
        tmp[3] = (1.0f / f1) * 0.5f;
    } else {
        const int next[3] = {1, 2, 0};
        int big = (m[0] < m[5]) ? 1 : 0;
        int i;
        int j;
        int k;
        if (m[big * 5] < m[10]) {
            big = 2;
        }
        i = next[big];
        j = next[i];
        f1 = (m[big * 5] - (m[i * 5] + m[j * 5])) + 1.0f;
        f1 = hkRsqrtApprox(f1);
        f2 = 0.5f / (1.0f / f1);
        k = big;
        tmp[k] = (1.0f / f1) * 0.5f;
        tmp[3] = f2 * (m[i * 4 + j] - m[j * 4 + i]);
        tmp[i] = f2 * (m[big * 4 + i] + m[i * 4 + big]);
        tmp[j] = f2 * (m[big * 4 + j] + m[j * 4 + big]);
    }
    out->m_vec.x = tmp[0];
    out->m_vec.y = tmp[1];
    out->m_vec.z = tmp[2];
    out->m_vec.w = tmp[3];
}

void hkQuaternion::set(const hkRotation& r) {
    hkQuadReal::quaternionFromRotatation(this, r);
}

// Returns a unit-length vector perpendicular to the input (a rotation by 90 degrees about a coordinate axis).
void hkQuaternion::setFlippedRotation(const hkQuaternion& q) {
    const hkReal* in = &q.m_vec.x;
    hkVector4 tmpVec;
    hkReal* tmp = &tmpVec.x;
    hkReal fVar1;
    int bVar5 = 1;
    int iMin = 0;
    int iOther = 2;
    hkReal a0 = (hkReal)fabs(in[0]);
    hkReal a1 = (hkReal)fabs(in[1]);
    fVar1 = a0;
    if (a1 < a0) {
        fVar1 = a1;
        bVar5 = 0;
        iMin = 1;
    }
    hkReal a2 = (hkReal)fabs(in[2]);
    if (a2 < fVar1) {
        iMin = 2;
        iOther = !bVar5;
    }
    fVar1 = 0.0f;
    tmp[iMin] = 0.0f;
    tmpVec.w = 0.0f;
    hkReal fVar2 = in[bVar5];
    tmp[bVar5] = in[iOther];
    tmp[iOther] = -fVar2;
    fVar2 = tmp[1] * tmp[1] + tmp[0] * tmp[0] + tmp[2] * tmp[2];
    if (fVar2 != 0.0f) {
        if (fVar2 <= 0.0f) {
            fVar1 = hkInfinity();
        } else {
            fVar1 = (hkReal)__frsqrte(fVar2);
            fVar1 = (0.5f * fVar1) * -(fVar1 * (fVar2 * fVar1) - 3.0f);
        }
    }
    tmpVec.set(tmp[0] * fVar1, tmp[1] * fVar1, tmp[2] * fVar1, 0.0f);
    m_vec = tmpVec;
}
