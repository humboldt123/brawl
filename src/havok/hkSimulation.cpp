// Havok translation unit hkSimulation.o (main.dol 0x802F0BA8-0x802F30BC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802F0BA8    68  __ct   [map: hkSimulation____ct]
//   0x802F0BEC    92  __dt   [map: hkSimulation____dt]
//   0x802F0C48   356  integrate   [map: hkSimulation__integrate]
//   0x802F0DAC   416  collide   [map: hkSimulation__collide]
//   0x802F0F4C   568  reCollideAfterStepFailure   [map: hkSimulation__reCollideAfterStepFailure]
//   0x802F1184    92  snapSimulateTimeAndGetTimeToAdvanceTo   [map: hkSimulation__snapSimulateTimeAndGetTimeToAdvanceTo]
//   0x802F11E0   248  advanceTime   [map: hkSimulation__advanceTime]
//   0x802F12D8   292  stepDeltaTime   [map: hkSimulation__stepDeltaTime]
//   0x802F13FC   132  __dt   [map: hkTimeFunctionHelper____dt]
//   0x802F1480  1764  collideEntitiesBroadPhaseDiscrete   [map: hkSimulation__collideEntitiesBroadPhaseDiscrete]
//   0x802F1B64  1104  collideInternal   [map: hkSimulation__collideInternal]
//   0x802F1FB4  1544  integrateInternal   [map: hkSimulation__integrateInternal]
//   0x802F25BC   148  applyActions   [map: hkSimulation__applyActions]
//   0x802F2650   264  collideEntitiesDiscrete   [map: hkSimulation__collideEntitiesDiscrete]
//   0x802F2758    68  collideEntitiesNarrowPhaseDiscrete   [map: hkSimulation__collideEntitiesNarrowPhaseDiscrete]
//   0x802F279C   280  processAgentCollideDiscrete   [map: hkSimulation__processAgentCollideDiscrete]
//   0x802F28B4     8  getCollidableA   [map: hkAgentNnEntry__getCollidableA]
//   0x802F28BC    84  resetCollisionInformationForEntities   [map: hkSimulation__resetCollisionInformationForEntities]
//   0x802F2910    12  processAgentResetCollisionInformation   [map: hkSimulation__processAgentResetCollisionInformation]
//   0x802F291C  1312  processAgentsOfEntities   [map: hkSimulation__processAgentsOfEntities]
//   0x802F2E3C     8  getLinkedCollidable   [map: hkEntity__getLinkedCollidable]
//   0x802F2E44    36  __ct   [map: hkQuaternion____ct1]
//   0x802F2E68     8  getRotation   [map: hkMotion__getRotation]
//   0x802F2E70     8  getRigidMotion   [map: hkRigidBody__getRigidMotion]
//   0x802F2E78     8  getTransform   [map: hkRigidBody__getTransform]
//   0x802F2E80     8  getSeparatingNormal   [map: hkContactPoint__getSeparatingNormal]
//   0x802F2E88     8  getCollidable   [map: hkWorldObject__getCollidable]
//   0x802F2E90    52  setNeg4   [map: hkVector4__setNeg4]
//   0x802F2EC4     8  getCenterOfMassInWorld   [map: hkRigidBody__getCenterOfMassInWorld]
//   0x802F2ECC     4  getPosition   [map: hkContactPoint__getPosition1]
//   0x802F2ED0    88  asin   [map: hkMath__asin]
//   0x802F2F28    48  __ct   [map: hkQuaternion____ct2]
//   0x802F2F58   228  setRotationAroundCentreOfMass   [map: hkRigidBody__setRotationAroundCentreOfMass]
//   0x802F303C   116  setInverseMul   [map: hkQuaternion__setInverseMul]
//   0x802F30B0    12  __ct   [map: hkProcessCollisionOutput____ct]

#include <havok/hkSimulation.h>
#include <havok/hkRigidBody.h>

hkSimulation::hkSimulation(hkWorld* world) {
    m_world = world;
    m_unk10 = 0.0f;
    m_unk14 = 0.0f;
    m_unk1C = 1.0f;
    m_unk20 = -1.0f;
    m_unk24 = 0;
    m_unk0C = 1;
}

hkSimulation::~hkSimulation() {}

// Snaps the current time to the step end when it is close enough, then returns the time to advance to.
hkReal hkSimulation::snapSimulateTimeAndGetTimeToAdvanceTo() {
    if (0.0f != m_unk1C) {
        hkReal diff = m_unk1C - m_unk14;
        if ((hkReal)fabs(diff) < m_unk20) {
            m_unk1C = m_unk14;
        }
    }
    if (m_unk1C == 0.0f) {
        return m_unk14;
    }
    if (m_unk14 < m_unk1C) {
        return m_unk14;
    }
    return m_unk1C;
}

// Accessors (8 bytes each), emitted as out-of-line functions in this TU.
hkLinkedCollidable* hkEntity::getLinkedCollidable() {
    return &m_collidable;
}

hkMotion* hkRigidBody::getRigidMotion() {
    return getMotion();
}

hkTransform& hkRigidBody::getTransform() {
    return *(hkTransform*)((u8*)this + 0xB0);
}

hkVector4& hkRigidBody::getCenterOfMassInWorld() {
    return *(hkVector4*)((u8*)this + 0x100);
}
