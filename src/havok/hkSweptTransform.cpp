// Havok translation unit hkSweptTransform.o (main.dol 0x80285CCC-0x80285D40).
//   0x80285CCC   116  initSweptTransform
#include <havok/hkSweptTransform.h>

void hkSweptTransform::initSweptTransform(const hkVector4& position, const hkQuaternion& rotation) {
    m_centerOfMass0.setXYZ0(position); // w: base time
    m_centerOfMass1.setXYZ0(position); // w: inverse delta time
    m_rotation0 = rotation;
    m_rotation1 = rotation;
    m_centerOfMassLocal.setZero4();
}
