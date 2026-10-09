// Havok translation unit hkSimpleContactConstraintInfo.o (main.dol 0x80293CBC-0x80295108).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80293CBC  1720  hkSimpleContactConstraintDataBuildJacobian   [map: hkSimpleContactConstraintInfo__hkSimpleContactConstraintDataBuildJacobian]
//   0x80294374    12  getContactPoints   [map: hkSimpleContactConstraintAtom__getContactPoints]
//   0x80294380    24  getContactPointProperties   [map: hkSimpleContactConstraintAtom__getContactPointProperties]
//   0x80294398    24  setZero4   [map: hkVector4__setZero4]
//   0x802943B0    36  initHeader   [map: hkJacobianHeaderSchema__initHeader]
//   0x802943D4     8  hkGetByteOffset   [map: hkSimpleContactConstraintInfo__hkGetByteOffset]
//   0x802943DC     4  initBuilder   [map: hkJacobianBuilder__initBuilder]
//   0x802943E0     4  as1Lin2Ang   [map: hkJacobianElement__as1Lin2Ang]
//   0x802943E4   860  buildLinearBegin   [map: hkJacobianBuilder__buildLinearBegin]
//   0x80294740    36  __as   [map: hkVector4____as]
//   0x80294764    68  setSub4   [map: hkVector4__setSub4]
//   0x802947A8    72  setCross   [map: hkVector4__setCross]
//   0x802947F0     4  getPosition   [map: hkContactPoint__getPosition]
//   0x802947F4     8  getNormal   [map: hkContactPoint__getNormal]
//   0x802947FC     8  getFriction8_8   [map: hkContactPointMaterial__getFriction8_8]
//   0x80294804   116  calculateRhs   [map: hkContactPoint__calculateRhs]
//   0x80294878     8  getDistance   [map: hkContactPoint__getDistance]
//   0x80294880    12  buildLinearEnd   [map: hkJacobianBuilder__buildLinearEnd]
//   0x8029488C   132  addLastPosition   [map: hkJacobianBuilder__addLastPosition]
//   0x80294910     4  copyJacRegToJac1Reg   [map: hkJacobianBuilder__copyJacRegToJac1Reg]
//   0x80294914    16  initSingleContact   [map: hkJacobianSingleContactSchema__initSingleContact]
//   0x80294924   772  getInvJac01Optimized   [map: hkJacobianBuilder__getInvJac01Optimized]
//   0x80294C28    20  initPairContact   [map: hkJacobianPairContactSchema__initPairContact]
//   0x80294C3C    28  lengthSquared3   [map: hkVector4__lengthSquared3]
//   0x80294C58    40  dot3   [map: hkVector4__dot3]
//   0x80294C80   156  normalize3   [map: hkVector4__normalize3]
//   0x80294D1C    52  mul4   [map: hkVector4__mul4]
//   0x80294D50    12  getColumn   [map: hkTransform__getColumn]
//   0x80294D5C    12  getIdentity   [map: hkTransform__getIdentity]
//   0x80294D68    68  setAbs4   [map: hkVector4__setAbs4]
//   0x80294DAC    40  init2dFriction   [map: hkJacobian2dFrictionSchema__init2dFriction]
//   0x80294DD4   660  buildAngularBegin   [map: hkJacobianBuilder__buildAngularBegin]
//   0x80295068    20  initAngular   [map: hkJacobian3dFrictionSchema__initAngular]
//   0x8029507C    12  buildAngularEnd   [map: hkJacobianBuilder__buildAngularEnd]
//   0x80295088    16  mulInvJacDiag   [map: hkJacobianBuilder__mulInvJacDiag]
//   0x80295098   108  length3   [map: hkVector4__length3]
//   0x80295104     4  exitBuilder   [map: hkJacobianBuilder__exitBuilder]

#pragma fp_contract on
#include <havok/hkSimpleContactConstraintInfo.h>
#include <havok/hkSimpleContactConstraintAtom.h>
#include <havok/hkContactPoint.h>
#include <havok/hkContactPointMaterial.h>
#include <havok/hkJacobianHeaderSchema.h>
#include <havok/hkJacobianBuilder.h>
#include <havok/hkJacobianElement.h>
#include <havok/hkJacobianSingleContactSchema.h>
#include <havok/hkJacobianPairContactSchema.h>
#include <havok/hkJacobian3dFrictionSchema.h>
#include <havok/hkJacobian2dFrictionSchema.h>

hkContactPoint* hkSimpleContactConstraintAtom::getContactPoints() {
    return (hkContactPoint*)(((u32)this + 0x37) & ~0xF);
}

hkContactPointMaterial* hkSimpleContactConstraintAtom::getContactPointProperties() {
    return (hkContactPointMaterial*)((u8*)getContactPoints() + (m_numContactPoints << 5));
}

void hkJacobianHeaderSchema::initHeader(u32 a, u32 b, u32 c, u32 d, u32 e) {
    m_tag = 0x01010018;
    unk04 = c;
    unk08 = a;
    unk0C = b;
    unk14 = d;
    unk10 = e;
}

int hkSimpleContactConstraintInfo::hkGetByteOffset(const void* base, const void* ptr) {
    return (u8*)ptr - (u8*)base;
}

void hkJacobianBuilder::initBuilder() {}

void hkJacobianElement::as1Lin2Ang() {}

hkVector4* hkContactPoint::getPosition() {
    return &m_position;
}

hkVector4* hkContactPoint::getNormal() {
    return &m_normal;
}

void hkJacobianBuilder::exitBuilder() {}

u16 hkContactPointMaterial::getFriction8_8() {
    return m_friction;
}

hkReal hkContactPoint::getDistance() {
    return m_normal.w;
}

void hkJacobianBuilder::copyJacRegToJac1Reg() {}

void hkJacobianSingleContactSchema::initSingleContact() {
    m_tag = 0x03090004;
}

void hkJacobianPairContactSchema::initPairContact(float f) {
    unk04 = f;
    m_tag = 0x040C0008;
}

void hkJacobian3dFrictionSchema::initAngular(float f) {
    unk18 = f;
    m_tag = 0x090D001C;
}

void hkJacobianBuilder::mulInvJacDiag(float s) {
    m_lastPosition.w = s * m_lastPosition.w;
}

void hkJacobianBuilder::buildAngularEnd(float a, float b) {
    unk10.w = a * b;
}

void hkJacobianBuilder::buildLinearEnd(float a, float b) {
    m_lastPosition.w = a * b;
}

void hkJacobianBuilder::addLastPosition(const hkVector4* b, hkVector4* c, hkVector4* d) {
    c->x += m_lastPosition.x;
    c->y += m_lastPosition.y;
    c->z += m_lastPosition.z;
    c->w += m_lastPosition.w;
    d->x += b->x;
    d->y += b->y;
    d->z += b->z;
    d->w += b->w;
}

void hkJacobian2dFrictionSchema::init2dFriction(u32 a, u32 b, float c, float d) {
    unk08 = d;
    m_tag = 0x080C0018;
    unk04 = a;
    unk0C = c;
    unk10 = 1.0f;
    unk14 = b;
}
