// Havok translation unit hkFootPlacementIkSolver.o (main.dol 0x8032D764-0x8032DA40).
// Partly decompiled (getBoneModelSpace). Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8032D764   520  setRotatedDir   [map: hkVector4__setRotatedDir1]
//   0x8032D96C     8  getRotation   [map: hkQsTransform__getRotation]
//   0x8032D974     4  getTranslation   [map: hkQsTransform__getTranslation]
//   0x8032D978    36  getBoneModelSpace   [map: hkPose__getBoneModelSpace]
//   0x8032D99C   164  normalize   [map: hkQuaternion__normalize]

#include <havok/hkPose.h>
#include <havok/hkQsTransform.h>
#include <havok/hkQuaternion.h>

#pragma fp_contract on

hkVector4& hkQsTransform::getTranslation() {
    return m_translation;
}

hkQuaternion& hkQsTransform::getRotation() {
    return m_rotation;
}

void hkQuaternion::normalize() {
    hkReal len2 = m_vec.x * m_vec.x + m_vec.y * m_vec.y + m_vec.z * m_vec.z + m_vec.w * m_vec.w;
    hkReal inv = 0.0f;
    if (len2 == 0.0f) {
    } else if (len2 <= 0.0f) {
        union {
            u32 i;
            hkReal f;
        } inf;
        inf.i = 0x7F800000;
        inv = inf.f;
    } else {
        // Hardware reciprocal square root estimate refined by one Newton step.
        hkReal y0 = (hkReal)__frsqrte(len2);
        inv = (0.5f * y0) * (1.5f - y0 * (len2 * y0));
    }
    m_vec.x *= inv;
    m_vec.y *= inv;
    m_vec.z *= inv;
    m_vec.w *= inv;
}

hkQsTransform* hkPose::getBoneModelSpace(int boneIndex) {
    if (m_boneFlags[boneIndex] & BONE_FLAG_LOCAL_DIRTY) {
        return calculateBoneModelSpace(boneIndex);
    }
    return &m_modelPose[boneIndex];
}
