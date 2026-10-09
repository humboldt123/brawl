// Havok translation unit hkQsTransform.o (main.dol 0x80284DB0-0x8028564C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80284DB0   380  get4x4ColumnMajor   [map: hkQsTransform__get4x4ColumnMajor]
//   0x80284F2C   192  set4x4ColumnMajor   [map: hkQsTransform__set4x4ColumnMajor]
//   0x80284FEC   640  setInverse   [map: hkQsTransform__setInverse]
//   0x8028526C   900  setMul   [map: hkQsTransform__setMul]
#include <havok/hkQsTransform.h>
#include <havok/hkTransform.h>

// Builds the 4x4 of (rotation * scale, translation). The original builds a hkTransform on the stack and
// calls hkTransform::get4x4ColumnMajor (fn_802871F4). Its matrix product goes through hkMatrix3::setMul
// (fn_80282CD4), which is written out here as column scaling.
void hkQsTransform::get4x4ColumnMajor(hkReal* m) const {
    hkRotation rotation;
    rotation.set(m_rotation);

    hkTransform t;
    t.m_rotation.m_col0.set(rotation.m_col0.x * m_scale.x, rotation.m_col0.y * m_scale.x, rotation.m_col0.z * m_scale.x, 0.0f);
    t.m_rotation.m_col1.set(rotation.m_col1.x * m_scale.y, rotation.m_col1.y * m_scale.y, rotation.m_col1.z * m_scale.y, 0.0f);
    t.m_rotation.m_col2.set(rotation.m_col2.x * m_scale.z, rotation.m_col2.y * m_scale.z, rotation.m_col2.z * m_scale.z, 0.0f);
    t.m_translation = m_translation;
    t.get4x4ColumnMajor(m);
}
