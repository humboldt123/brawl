// Havok translation unit hkRigidBody.o (main.dol 0x802E22C4-0x802E3604).
// Functions in address order (method names from the Havok TU map):
//   0x802E22C4    76  finishLoadedObjecthkRigidBody   [map: hkRigidBody__finishLoadedObjecthkRigidBody]
//   0x802E2310    20  cleanupLoadedObjecthkRigidBody   [map: hkRigidBody__cleanupLoadedObjecthkRigidBody]
//   0x802E2324    84  getVtablehkRigidBody   [map: hkRigidBody__getVtablehkRigidBody]
//   0x802E2378    84  estimateAllowedPenetrationDepth   [map: hkCollidable__estimateAllowedPenetrationDepth]
//   0x802E23CC   476  updateCachedShapeInfo   [map: hkRigidBody__updateCachedShapeInfo]
//   0x802E25A8   664  createDynamicRigidMotion   [map: hkRigidBody__createDynamicRigidMotion]
//   0x802E2840   604  __ct   [map: hkRigidBody____ct]
//   0x802E2A9C   112  __dt   [map: hkRigidBody____dt]
//   0x802E2B0C   412  getCinfo   [map: hkRigidBody__getCinfo]
//   0x802E2CA8     8  getMotionState   [map: hkRigidBody__getMotionState]
//   0x802E2CB0     4  setDeactivator   [map: hkRigidBody__setDeactivator]
//   0x802E2CB4   836  clone   [map: hkRigidBody__clone]
//   0x802E2FF8    96  setMotionType   [map: hkRigidBody__setMotionType]
//   0x802E3058   464  setShape   [map: hkRigidBody__setShape]
//   0x802E3228    92  setCenterOfMassLocal   [map: hkRigidBody__setCenterOfMassLocal]
//   0x802E3284   268  setDeactivator   [map: hkRigidBody__setDeactivator1]
//   0x802E3390   328  updateBroadphaseAndResetCollisionInformationOfWarpedBody   [map: hkRigidBody__updateBroadphaseAndResetCollisionInformationOfWarpedBody]
//   0x802E34D8    68  setPosition   [map: hkRigidBody__setPosition]
//   0x802E351C    68  setPositionAndRotation   [map: hkRigidBody__setPositionAndRotation]
//   0x802E3560    68  setTransform   [map: hkRigidBody__setTransform]
//   0x802E35A4    16  setMass   [map: hkRigidBody__setMass]
//   0x802E35B4    80  __sinit_\hkRigidBody_cpp   [map: hkRigidBodycpp____sinit_]

#include <new>
#include <havok/hkRigidBody.h>

void hkRigidBody::finishLoadedObjecthkRigidBody(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    new (p) hkRigidBody(flag);
}

void hkRigidBody::cleanupLoadedObjecthkRigidBody(void* p) {
    ((hkRigidBody*)p)->~hkRigidBody();
}

const void* hkRigidBody::getVtablehkRigidBody() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    hkVector4 buf[31]; // 0x1F0 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkRigidBody(flag);
    return *(const void**)buf;
}

hkMotionState* hkRigidBody::getMotionState() {
    return &getMotion()->m_motionState;
}

void hkRigidBody::setDeactivator(hkEntityDeactivator* deactivator) {
    hkEntity::setDeactivator(deactivator);
}

void hkRigidBody::setPosition(const hkVector4& position) {
    getMotion()->setPosition(position);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

void hkRigidBody::setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation) {
    getMotion()->setPositionAndRotation(position, rotation);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

void hkRigidBody::setTransform(const hkTransform& transform) {
    getMotion()->setTransform(transform);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

void hkRigidBody::setMass(hkReal mass) {
    getMotion()->setMass(mass);
}
