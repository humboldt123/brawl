// Havok translation unit hkSpatialRigidBodyDeactivator.o (main.dol 0x802E37E0-0x802E3C18).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E37E0    32  finishLoadedObjecthkSpatialRigidBodyDeactivator   [map: hkSpatialRigidBodyDeactivator__finishLoadedObjecthkSpatialRigidBodyDeactivator]
//   0x802E3800    20  cleanupLoadedObjecthkSpatialRigidBodyDeactivator   [map: hkSpatialRigidBodyDeactivator__cleanupLoadedObjecthkSpatialRigidBodyDeactivator]
//   0x802E3814    92  __dt   [map: hkSpatialRigidBodyDeactivator____dt]
//   0x802E3870    60  getVtablehkSpatialRigidBodyDeactivator   [map: hkSpatialRigidBodyDeactivator__getVtablehkSpatialRigidBodyDeactivator]
//   0x802E38AC   164  __ct   [map: hkSpatialRigidBodyDeactivator____ct]
//   0x802E3950     8  getRigidBodyDeactivatorType   [map: hkSpatialRigidBodyDeactivator__getRigidBodyDeactivatorType]
//   0x802E3958   312  shouldDeactivateHighFrequency   [map: hkSpatialRigidBodyDeactivator__shouldDeactivateHighFrequency]
//   0x802E3A90   312  shouldDeactivateLowFrequency   [map: hkSpatialRigidBodyDeactivator__shouldDeactivateLowFrequency]
//   0x802E3BC8    80  __sinit_\hkSpatialRigidBodyDeactivator_cpp   [map: hkSpatialRigidBodyDeactivatorcpp____sinit_]
#include <new>
#include <havok/hkSpatialRigidBodyDeactivator.h>
#include <havok/hkEntity.h>
#include <havok/hkRegistry.h>
#include <havok/hkMath.h>
#include <havok/hkCdPoint.h> // HK_REAL_MAX

void hkSpatialRigidBodyDeactivator::finishLoadedObjecthkSpatialRigidBodyDeactivator(void* p) {
    new (p) hkSpatialRigidBodyDeactivator(hkFinishLoadedObjectFlag());
}

void hkSpatialRigidBodyDeactivator::cleanupLoadedObjecthkSpatialRigidBodyDeactivator(void* p) {
    ((hkSpatialRigidBodyDeactivator*)p)->~hkSpatialRigidBodyDeactivator();
}

hkSpatialRigidBodyDeactivator::~hkSpatialRigidBodyDeactivator() {}

#pragma push
#pragma auto_inline off
const void* hkSpatialRigidBodyDeactivator::getVtablehkSpatialRigidBodyDeactivator() {
    hkVector4 buf[7]; // placed object storage (0x70 bytes, 16-byte aligned)
    new (buf) hkSpatialRigidBodyDeactivator(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}
#pragma pop

hkSpatialRigidBodyDeactivator::hkSpatialRigidBodyDeactivator() {
    m_radiusSqrd = -1.0f;
    m_minHighFrequencyTranslation = 0.01f;
    m_minHighFrequencyRotation = 0.005f;
    m_minLowFrequencyTranslation = 0.1f;
    m_minLowFrequencyRotation = 0.2f;
    m_highFrequencySample.m_refPosition.setAll(HK_REAL_MAX);
    m_lowFrequencySample.m_refPosition.setAll(HK_REAL_MAX);
    m_highFrequencySample.m_refRotation.m_vec.set(0.0f, 0.0f, 0.0f, 1.0f);
    m_lowFrequencySample.m_refRotation.m_vec.set(0.0f, 0.0f, 0.0f, 1.0f);
}

int hkSpatialRigidBodyDeactivator::getRigidBodyDeactivatorType() const {
    return DEACTIVATOR_SPATIAL;
}

hkBool hkSpatialRigidBodyDeactivator::shouldDeactivateHighFrequency(hkEntity* entity) {
    hkMotionState& ms = entity->m_motion.m_motionState;
    hkVector4& position = ms.m_sweptTransform.m_centerOfMass1;
    hkQuaternion& rotation = ms.m_sweptTransform.m_rotation1;
    hkVector4 dPos;
    dPos.setSub4(m_highFrequencySample.m_refPosition, position);
    hkVector4 dRot;
    dRot.setSub4(m_highFrequencySample.m_refRotation.m_vec, rotation.m_vec);
    hkReal radius = ms.m_objectRadius;
    hkReal distSqrd = dPos.lengthSquared3();
    hkReal rotSqrd = radius * (radius * dRot.lengthSquared4());
    if (distSqrd < m_minHighFrequencyTranslation * m_minHighFrequencyTranslation &&
        rotSqrd < m_minHighFrequencyRotation * m_minHighFrequencyRotation) {
        return true;
    }
    m_highFrequencySample.m_refPosition = position;
    m_highFrequencySample.m_refRotation = rotation;
    return false;
}

hkBool hkSpatialRigidBodyDeactivator::shouldDeactivateLowFrequency(hkEntity* entity) {
    hkMotionState& ms = entity->m_motion.m_motionState;
    hkVector4& position = ms.m_sweptTransform.m_centerOfMass1;
    hkQuaternion& rotation = ms.m_sweptTransform.m_rotation1;
    hkVector4 dPos;
    dPos.setSub4(m_lowFrequencySample.m_refPosition, position);
    hkVector4 dRot;
    dRot.setSub4(m_lowFrequencySample.m_refRotation.m_vec, rotation.m_vec);
    hkReal radius = ms.m_objectRadius;
    hkReal distSqrd = dPos.lengthSquared3();
    hkReal rotSqrd = radius * (radius * dRot.lengthSquared4());
    if (distSqrd < m_minLowFrequencyTranslation * m_minLowFrequencyTranslation &&
        rotSqrd < m_minLowFrequencyRotation * m_minLowFrequencyRotation) {
        return true;
    }
    m_lowFrequencySample.m_refPosition = position;
    m_lowFrequencySample.m_refRotation = rotation;
    return false;
}

static hkStaticTypeInfo hkSpatialRigidBodyDeactivatorTypeInfo("hkSpatialRigidBodyDeactivator",
    hkSpatialRigidBodyDeactivator::finishLoadedObjecthkSpatialRigidBodyDeactivator,
    hkSpatialRigidBodyDeactivator::cleanupLoadedObjecthkSpatialRigidBodyDeactivator,
    hkSpatialRigidBodyDeactivator::getVtablehkSpatialRigidBodyDeactivator());
