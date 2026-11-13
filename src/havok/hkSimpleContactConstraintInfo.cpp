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
#include <havok/hkConstraintQueryIn.h>
#include <havok/hkTransform.h>

// MATCH-ONLY: the target builds this function's vector locals without calling the empty hkVector4 constructor,
// which every other Havok function compiled without inlining does call; untyped aligned storage reproduces it.
// The original declaration of these locals is unknown. Inside the function each name below stands for its storage.
struct hkVector4Storage {
    hkReal v[4];
} __attribute__((aligned(16)));
#define posSum (*(hkVector4*)&posSumStorage)
#define normalSum (*(hkVector4*)&normalSumStorage)
#define axis (*(hkVector4*)&axisStorage)
#define perp (*(hkVector4*)&perpStorage)
#define absNormal (*(hkVector4*)&absNormalStorage)
#define arm (*(hkVector4*)&armStorage)

// Builds the Jacobians of a simple contact atom: one row per contact point (pairs of points flagged in their
// material share a pair schema), then, when the summed friction is positive, two linear friction rows through the
// centre of the points and, for more than one point, an angular friction row around the averaged normal.
// Compiled without inlining (every vector accessor is an out-of-line call).
#pragma push
#pragma dont_inline on
void hkSimpleContactConstraintDataBuildJacobian(hkSimpleContactConstraintAtom* atom, const hkConstraintQueryIn& in,
                                                hkConstraintQueryOut& out) {
    u8* schema;
    hkContactPoint* contactPoints;
    int numPoints;
    const hkVelocityAccumulator* bodyA;
    const hkVelocityAccumulator* bodyB;
    hkContactPointProperties* props;
    int unused = 0; // HYPOTHESIS: only its address is passed to calculateRhs
    contactPoints = atom->getContactPoints();
    numPoints = atom->m_numContactPoints;
    props = atom->getContactPointProperties();

    hkVector4Storage posSumStorage;
    posSum.setZero4();
    hkVector4Storage normalSumStorage;
    normalSum.setZero4();
    hkReal invNumPoints = 1.0f / numPoints;
    hkReal impulseSum = 0.0f;

    hkJacobianHeaderSchema* header = (hkJacobianHeaderSchema*)(hkJacobianSchema*)out.m_jacobianSchemas;
    bodyA = in.m_bodyA;
    bodyB = in.m_bodyB;
    header->initHeader(in.m_accumulatorAOffset, in.m_accumulatorBOffset,
                     hkSimpleContactConstraintInfo::hkGetByteOffset(out.m_jacobiansBase, out.m_jacobians), (u32)props,
                     0x14);
    schema = (u8*)(header + 1);
    hkJacobianBuilder::initBuilder(bodyA, bodyB, in.m_builderParam);

    {
    hkJacobianBuilder* jac = ((hkJacobianElement*)out.m_jacobians)->as1Lin2Ang();
    const hkContactPoint* cp = contactPoints;
    int frictionSum = 0;
    hkReal pairCoupling = 0.99f; // HYPOTHESIS: name (kept in a register across the loop)
    {
        for (int i = numPoints - 1; i >= 0; cp++, i--, props++) {
            hkJacobianBuilder::buildLinearBegin(bodyA, bodyB, cp->getPosition(), cp->getNormal(), jac, in.m_tau);
            frictionSum += props->getFriction8_8();
            impulseSum += props->m_impulseApplied;
            hkReal rhs = -cp->calculateRhs(props, &unused, cp->getDistance(), in.m_rhsParam);
            jac->buildLinearEnd(rhs, in.m_contactRhsScale);
            hkJacobianBuilder::addLastPosition(&cp->getPosition(), &cp->getNormal(), &posSum, &normalSum);
            if (!(props->m_flags & 2)) {
                hkJacobianBuilder::copyJacRegToJac1Reg();
                ((hkJacobianSingleContactSchema*)schema)->initSingleContact();
                schema += sizeof(hkJacobianSingleContactSchema);
                jac++;
            } else {
                // The previous point's single schema becomes a pair schema.
                hkJacobianPairContactSchema* pair =
                    (hkJacobianPairContactSchema*)(schema - sizeof(hkJacobianSingleContactSchema));
                pair->initPairContact(hkJacobianBuilder::getInvJac01Optimized(bodyA, bodyB, jac - 1, in.m_tau, pairCoupling));
                schema = (u8*)(pair + 1);
                jac++;
            }
        }
    }
    out.m_jacobians = (hkJacobianElement*)jac;

    hkReal maxFriction = invNumPoints * (impulseSum * ((1.0f / 256.0f) * frictionSum));
    if (maxFriction > 0.0f) {
        hkReal invNumPoints2 = invNumPoints * invNumPoints;
        hkSimdReal lenSq = normalSum.lengthSquared3();
        if (lenSq * invNumPoints2 > 0.9999f) {
            normalSum = contactPoints->getNormal();
        } else if (lenSq > 0.1f) {
            normalSum.normalize3();
        } else {
            normalSum = contactPoints->getNormal();
            if (normalSum.dot3(normalSum) < 0.9f) {
                maxFriction = 0.0f;
                normalSum.setZero4();
                normalSum(1) = 1.0f;
            }
        }

        hkVector4Storage axisStorage;
        hkVector4Storage perpStorage;
        for (;;) {
            axis = hkTransform::getIdentity().getColumn(atom->m_info.m_index);
            perp.setCross(axis, normalSum);
            bool ok = perp.dot3(perp) > 0.1f;
            if (ok) {
                perp.normalize3();
                break;
            }
            // The axis is (nearly) parallel to the normal: switch to the axis of the smallest normal component.
            hkVector4Storage absNormalStorage;
            absNormal.setAbs4(normalSum);
            if (absNormal(0) < absNormal(1)) {
                atom->m_info.m_index = absNormal(0) < absNormal(2) ? 0 : 2;
            } else {
                atom->m_info.m_index = absNormal(1) < absNormal(2) ? 1 : 2;
            }
            atom->m_info.m_frictionRhs0 = 0.0f;
            atom->m_info.m_frictionRhs1 = 0.0f;
        }
        axis.setCross(normalSum, perp);
        posSum.mul4(invNumPoints);

        hkJacobianBuilder* frictionJac = ((hkJacobianElement*)out.m_jacobians)->as1Lin2Ang();
        hkJacobianBuilder::buildLinearBegin(bodyA, bodyB, posSum, axis, frictionJac, in.m_tau);
        hkReal rhs0 = atom->m_info.m_frictionRhs0;
        frictionJac->buildLinearEnd(rhs0, in.m_frictionRhsScale);
        hkJacobianBuilder::copyJacRegToJac1Reg();
        hkJacobianBuilder::buildLinearBegin(bodyA, bodyB, posSum, perp, frictionJac + 1, in.m_tau);
        hkReal rhs1 = atom->m_info.m_frictionRhs1;
        (frictionJac + 1)->buildLinearEnd(rhs1, in.m_frictionRhsScale);
        frictionJac += 2;
        hkJacobian2dFrictionSchema* friction = (hkJacobian2dFrictionSchema*)schema;
        hkReal invJac = hkJacobianBuilder::getInvJac01Optimized(bodyA, bodyB, frictionJac - 2, in.m_tau, 1.0f);
        friction->init2dFriction((u32)&atom->m_info.m_data0C, 8, maxFriction * in.m_frictionScale, invJac);
        schema += sizeof(hkJacobian2dFrictionSchema);

        if (numPoints > 1) {
            for (;;) {
                if (!(atom->m_info.m_flags & 4)) {
                    hkJacobianBuilder::buildAngularBegin(bodyA, bodyB, normalSum, frictionJac, in.m_tau);
                    hkReal invRadius = 1.0f / atom->m_info.m_contactRadius;
                    ((hkJacobian3dFrictionSchema*)friction)->initAngular(atom->m_info.m_contactRadius);
                    schema = (u8*)friction + sizeof(hkJacobian3dFrictionSchema);
                    frictionJac->buildAngularEnd(atom->m_info.m_angularRhs, in.m_frictionRhsScale);
                    frictionJac->mulInvJacDiag(invRadius);
                    // The angular row has the two angular parts only (0x20 bytes).
                    frictionJac = (hkJacobianBuilder*)((u8*)frictionJac + 0x20);
                    break;
                }
                atom->m_info.m_contactRadius = 0.0f;
                {
                    const hkContactPoint* cp = contactPoints;
                    for (int i = numPoints - 1; i >= 0; i--, cp++) {
                        hkVector4Storage armStorage;
                        arm.setSub4(cp->getPosition(), posSum);
                        atom->m_info.m_contactRadius += arm.length3();
                    }
                }
                atom->m_info.m_contactRadius *= invNumPoints;
                if (atom->m_info.m_contactRadius < 1e-6f) {
                    break;
                }
                atom->m_info.m_flags &= ~4;
            }
        }
        out.m_jacobians = (hkJacobianElement*)frictionJac;
    }
    }
    out.m_jacobianSchemas = (hkJacobianSchema*)schema;
    hkJacobianBuilder::exitBuilder();
}
#pragma pop
#undef posSum
#undef normalSum
#undef axis
#undef perp
#undef absNormal
#undef arm

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

void hkJacobianBuilder::initBuilder(const hkVelocityAccumulator* bodyA, const hkVelocityAccumulator* bodyB,
                                    hkReal invDeltaTime) {}

hkJacobianBuilder* hkJacobianElement::as1Lin2Ang() {
    return (hkJacobianBuilder*)this;
}

const hkVector4& hkContactPoint::getPosition() const {
    return m_position;
}

const hkVector4& hkContactPoint::getNormal() const {
    return m_normal;
}

void hkJacobianBuilder::exitBuilder() {}

int hkContactPointMaterial::getFriction8_8() {
    return m_friction;
}

hkReal hkContactPoint::getDistance() const {
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

void hkJacobianBuilder::addLastPosition(const hkVector4* position, const hkVector4* normal, hkVector4* posSum,
                                        hkVector4* normalSum) {
    posSum->x += position->x;
    posSum->y += position->y;
    posSum->z += position->z;
    posSum->w += position->w;
    normalSum->x += normal->x;
    normalSum->y += normal->y;
    normalSum->z += normal->z;
    normalSum->w += normal->w;
}

void hkJacobian2dFrictionSchema::init2dFriction(u32 a, u32 b, float c, float d) {
    unk08 = d;
    m_tag = 0x080C0018;
    unk0C = c;
    unk04 = a;
    unk10 = 1.0f;
    unk14 = b;
}

// TODO: cleanup (shape from jj-link/brawl fn_80294924)
// fn_80294924: body from https://github.com/jj-link/brawl src/havok/fn_80294924.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
namespace jj_fn_80294924 {
#pragma fp_contract on
struct V { float x,y,z,w; } __attribute__((aligned(16)));
struct B { char pad[0x30]; V p; };
struct C { V a[3]; V b[3]; };
struct L { V p[3]; V wa, wb, t0, t1; };

extern "C" float fn_80294924(B* b0, B* b1, C* c, float s, float k) {
    L l;
    l.p[0].x = c->b[0].x * c->a[0].x; l.p[0].y = c->b[0].y * c->a[0].y;
    l.p[0].z = c->b[0].z * c->a[0].z; l.p[0].w = c->b[0].w * c->a[0].w;
    l.p[1].x = c->b[1].x * c->a[1].x; l.p[1].y = c->b[1].y * c->a[1].y;
    l.p[1].z = c->b[1].z * c->a[1].z; l.p[1].w = c->b[1].w * c->a[1].w;
    l.p[2].x = c->b[2].x * c->a[2].x; l.p[2].y = c->b[2].y * c->a[2].y;
    l.p[2].z = c->b[2].z * c->a[2].z; l.p[2].w = c->b[2].w * c->a[2].w;
    float wb = b1->p.w;
    float wa = b0->p.w;
    l.wa.x = wa; l.wa.y = wa; l.wa.z = wa; l.wa.w = wa;
    l.wb.x = wb; l.wb.y = wb; l.wb.z = wb; l.wb.w = wb;
    l.t0.x = l.wa.x * l.p[0].x; l.t0.y = l.wa.y * l.p[0].y; l.t0.z = l.wa.z * l.p[0].z; l.t0.w = l.wa.w * l.p[0].w;
    l.t1.x = l.wb.x * l.p[0].x; l.t1.y = l.wb.y * l.p[0].y; l.t1.z = l.wb.z * l.p[0].z; l.t1.w = l.wb.w * l.p[0].w;
    l.p[1].x *= b0->p.x; l.p[1].y *= b0->p.y; l.p[1].z *= b0->p.z; l.p[1].w *= l.wa.w;
    l.p[2].x *= b1->p.x; l.p[2].y *= b1->p.y; l.p[2].z *= b1->p.z; l.p[2].w *= l.wb.w;
    l.t0.x = l.t0.x + l.t1.x; l.t0.y = l.t0.y + l.t1.y; l.t0.z = l.t0.z + l.t1.z; l.t0.w = l.t0.w + l.t1.w;
    l.p[1].x = (l.p[1].x + l.p[2].x);
    l.p[1].y = (l.p[1].y + l.p[2].y);
    l.p[1].z = (l.p[1].z + l.p[2].z);
    l.p[1].w = (l.p[1].w + l.p[2].w);
    l.p[1].x = l.p[1].x + l.t0.x;
    l.p[1].y = l.p[1].y + l.t0.y;
    l.p[1].z = l.p[1].z + l.t0.z;
    l.p[1].w = l.p[1].w + l.t0.w;
    float d = l.p[1].z + (l.p[1].x + l.p[1].y);
    float f = d * k;
    float m0 = c->a[2].w;
    float m1 = c->b[2].w;
    float inv = s / (m0 * m1 - f * f);
    c->a[2].w = m1 * inv;
    c->b[2].w = m0 * inv;
    return -f * inv;
}
} // namespace jj_fn_80294924
