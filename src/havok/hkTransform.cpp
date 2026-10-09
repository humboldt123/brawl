// Havok translation unit hkTransform.o (main.dol 0x802871F4-0x802878F0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802871F4   128  get4x4ColumnMajor   [map: hkTransform__get4x4ColumnMajor]
//   0x80287274   128  set4x4ColumnMajor   [map: hkTransform__set4x4ColumnMajor]
//   0x802872F4   244  setInverse   [map: hkTransform__setInverse]
#include <havok/hkTransform.h>

// Column-major 4x4: m[col * 4 + row], with the w entries 0 and the translation column last.
void hkTransform::get4x4ColumnMajor(hkReal* m) const {
    m[0] = m_rotation.m_col0.x;
    m[1] = m_rotation.m_col0.y;
    m[2] = m_rotation.m_col0.z;
    m[3] = 0.0f;
    m[4] = m_rotation.m_col1.x;
    m[5] = m_rotation.m_col1.y;
    m[6] = m_rotation.m_col1.z;
    m[7] = 0.0f;
    m[8] = m_rotation.m_col2.x;
    m[9] = m_rotation.m_col2.y;
    m[10] = m_rotation.m_col2.z;
    m[11] = 0.0f;
    m[12] = m_translation.x;
    m[13] = m_translation.y;
    m[14] = m_translation.z;
    m[15] = 1.0f;
}

void hkTransform::set4x4ColumnMajor(const hkReal* m) {
    m_rotation.m_col0.x = m[0];
    m_rotation.m_col0.y = m[1];
    m_rotation.m_col0.z = m[2];
    m_rotation.m_col0.w = 0.0f;
    m_rotation.m_col1.x = m[4];
    m_rotation.m_col1.y = m[5];
    m_rotation.m_col1.z = m[6];
    m_rotation.m_col1.w = 0.0f;
    m_rotation.m_col2.x = m[8];
    m_rotation.m_col2.y = m[9];
    m_rotation.m_col2.z = m[10];
    m_rotation.m_col2.w = 0.0f;
    m_translation.x = m[12];
    m_translation.y = m[13];
    m_translation.z = m[14];
    m_translation.w = 1.0f;
}

// Inverse of a rigid transform: rotation transposed, translation -(R^T t).
// The original transposes through hkMatrix3::setTranspose (fn_80282C78); the transpose is written out here.
void hkTransform::setInverse(const hkTransform& t) {
    const hkRotation& src = t.m_rotation;
    m_rotation.m_col0.x = src.m_col0.x;
    m_rotation.m_col0.y = src.m_col1.x;
    m_rotation.m_col0.z = src.m_col2.x;
    m_rotation.m_col0.w = 0.0f;
    m_rotation.m_col1.x = src.m_col0.y;
    m_rotation.m_col1.y = src.m_col1.y;
    m_rotation.m_col1.z = src.m_col2.y;
    m_rotation.m_col1.w = 0.0f;
    m_rotation.m_col2.x = src.m_col0.z;
    m_rotation.m_col2.y = src.m_col1.z;
    m_rotation.m_col2.z = src.m_col2.z;
    m_rotation.m_col2.w = 0.0f;

    hkVector4 negTranslation;
    negTranslation.set(-t.m_translation.x, -t.m_translation.y, -t.m_translation.z, -t.m_translation.w);
    m_translation.setRotatedDir(m_rotation, negTranslation);
}
