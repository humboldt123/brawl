// Havok translation unit hkMotionState.o (main.dol 0x80281FB0-0x80282040).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80281FB0   144  initMotionState   [map: hkMotionState__initMotionState]
#include <havok/hkMotionState.h>

void hkMotionState::initMotionState(const hkVector4& position, const hkQuaternion& rotation) {
    m_sweptTransform.initSweptTransform();
    m_transform.m_rotation.set(rotation);
    m_transform.m_translation = position;
    m_deltaAngle.setZero4();
    m_objectRadius = 1.0f; // HYPOTHESIS: default radius constant
}
