// Havok translation unit hkSolver.o (main.dol 0x80296734-0x8029F098).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80296734     4  hkSolveUpload   [map: hkSolver__hkSolveUpload]
//   0x80296738     4  hkDebugPrintfAccumulators   [map: hkSolverInfo__hkDebugPrintfAccumulators]
//   0x8029673C  1948  hkSolver_solveStiffSpringChain   [map: hkSolver__hkSolver_solveStiffSpringChain]
//   0x80296ED8     8  hkAddByteOffset<17hkJacobianElement>   [map: hkJacobianElement__hkAddByteOffset_17hkJacobianElement_]
//   0x80296EE0    12  getAccumulator   [map: hkVelocityAccumulatorOffset__getAccumulator]
//   0x80296EEC     4  loadVelocityAccumulators   [map: hkSolver__loadVelocityAccumulators]
//   0x80296EF0  1364  getLinearDv0UserTau   [map: hkSolver__getLinearDv0UserTau]
//   0x80297444    68  add4   [map: hkVector4__add4]
//   0x80297488    12  __cl   [map: hkVector4____cl1]
//   0x80297494     4  popVelocityAccumulators   [map: hkSolver__popVelocityAccumulators]
//   0x80297498   648  applyImpulse   [map: hkSolver__applyImpulse]
//   0x80297720    52  setMul4   [map: hkVector4__setMul4]
//   0x80297754     4  storeVelocityAccumulators   [map: hkSolver__storeVelocityAccumulators]
//   0x80297758   828  hkSolver_solveBallAndSocketChain   [map: hkSolver__hkSolver_solveBallAndSocketChain]
//   0x80297A94     8  getAccumulatorOffsetsBase   [map: hkJacobianBallSocketChainSchema__getAccumulatorOffsetsBase]
//   0x80297A9C    24  getMatrixBuffer   [map: hkJacobianBallSocketChainSchema__getMatrixBuffer]
//   0x80297AB4    32  getTempBuffer   [map: hkJacobianBallSocketChainSchema__getTempBuffer]
//   0x80297AD4   108  _setMul3   [map: hkVector4___setMul3]
//   0x80297B40    12  getSimdAt   [map: hkVector4__getSimdAt]
//   0x80297B4C  1604  hkSolver_solvePoweredChain   [map: hkSolver__hkSolver_solvePoweredChain]
//   0x80298190     8  __ct   [map: hkChainSolverInfo____ct]
//   0x80298198    24  getAngularJacobians   [map: hkJacobianPoweredChainSchema__getAngularJacobians]
//   0x802981B0     8  getAccumulatorOffsetsBase   [map: hkJacobianPoweredChainSchema__getAccumulatorOffsetsBase]
//   0x802981B8    32  getMatrixBuffer   [map: hkJacobianPoweredChainSchema__getMatrixBuffer]
//   0x802981D8    20  getChildConstraintStatusBase   [map: hkJacobianPoweredChainSchema__getChildConstraintStatusBase]
//   0x802981EC    40  getTempBuffer   [map: hkJacobianPoweredChainSchema__getTempBuffer]
//   0x80298214    52  getVelocityBuffer   [map: hkJacobianPoweredChainSchema__getVelocityBuffer]
//   0x80298248     8  getAngularRhs   [map: hk2AngJacobian__getAngularRhs]
//   0x80298250     8  setAngularRhs   [map: hk2AngJacobian__setAngularRhs]
//   0x80298258     8  __opi   [map: hkPadSpu_i_____opi]
//   0x80298260    20  getState   [map: hk3dAngularMotorSolverInfo__getState]
//   0x80298274    40  setZero8   [map: hkVector8__setZero8]
//   0x8029829C   620  _setMul6   [map: hkVector8___setMul6]
//   0x80298508   132  setSub8   [map: hkVector8__setSub8]
//   0x8029858C   424  applyAngularImpulse   [map: hkSolver__applyAngularImpulse]
//   0x80298734     8  __as   [map: hkPadSpu_i_____as]
//   0x8029873C     8  __as   [map: hkPadSpuf_f_____as]
//   0x80298744     8  __as   [map: hkPadSpu_P18hk1Lin2AngJacobian_____as]
//   0x8029874C     8  __as   [map: hkPadSpu_P14hk2AngJacobian_____as]
//   0x80298754     8  __as   [map: hkPadSpu_P27hkVelocityAccumulatorOffset_____as]
//   0x8029875C     8  __as   [map: hkPadSpu_P21hkVelocityAccumulator_____as]
//   0x80298764     8  __as   [map: hkPadSpu_P30hkConstraintChainMatrix6Triple_____as]
//   0x8029876C     8  __as   [map: hkPadSpu_P26hk3dAngularMotorSolverInfo_____as]
//   0x80298774     8  __as   [map: hkPadSpu_P9hkVector8_____as]
//   0x8029877C     8  __opP14hk2AngJacobian   [map: hkPadSpu_P14hk2AngJacobian_____opP14hk2AngJacobian]
//   0x80298784     8  __opP26hk3dAngularMotorSolverInfo   [map: hkPadSpu_P26hk3dAngularMotorSolverInfo_____opP26hk3dAngularMotorSolverInfo]
//   0x8029878C     8  __opP9hkVector8   [map: hkPadSpu_P9hkVector8_____opP9hkVector8]
//   0x80298794     8  __opP30hkConstraintChainMatrix6Triple   [map: hkPadSpu_P30hkConstraintChainMatrix6Triple_____opP30hkConstraintChainMatrix6Triple]
//   0x8029879C     8  __opP27hkVelocityAccumulatorOffset   [map: hkPadSpu_P27hkVelocityAccumulatorOffset_____opP27hkVelocityAccumulatorOffset]
//   0x802987A4     8  __opP18hk1Lin2AngJacobian   [map: hkPadSpu_P18hk1Lin2AngJacobian_____opP18hk1Lin2AngJacobian]
//   0x802987AC  1788  hkSolveStepJacobians   [map: hkSolver__hkSolveStepJacobians]
//   0x80298EA8     4  loadFixedRegisters   [map: hkSolver__loadFixedRegisters]
//   0x80298EAC  2324  stepJacobian   [map: hkSolver__stepJacobian]
//   0x802997C0    12  getSchemaType   [map: hkJacobianSchema__getSchemaType]
//   0x802997CC     4  prefetchVelocityAccumulators   [map: hkSolver__prefetchVelocityAccumulators]
//   0x802997D0    12  getBodyA   [map: hkJacobianHeaderSchema__getBodyA]
//   0x802997DC    12  getBodyB   [map: hkJacobianHeaderSchema__getBodyB]
//   0x802997E8    12  getJacobian   [map: hkJacobianHeaderSchema__getJacobian]
//   0x802997F4     8  hkAddByteOffset<16hkJacobianSchema>   [map: hkJacobianSchema__hkAddByteOffset_16hkJacobianSchema_]
//   0x802997FC    12  getSchemaSize   [map: hkJacobianSchema__getSchemaSize]
//   0x80299808  1020  solveSingleContact   [map: hkSolver__solveSingleContact]
//   0x80299C04    12  next   [map: hk1Lin2AngJacobian__next]
//   0x80299C10  2736  solvePairContact   [map: hkSolver__solvePairContact]
//   0x8029A6C0  2248  solve3dFriction   [map: hkSolver__solve3dFriction]
//   0x8029AF88    12  next   [map: hk2AngJacobian__next]
//   0x8029AF94  1804  solve2dFriction   [map: hkSolver__solve2dFriction]
//   0x8029B6A0     4  as2Ang   [map: hkJacobianElement__as2Ang]
//   0x8029B6A4   624  solve1dAngFriction   [map: hkSolver__solve1dAngFriction]
//   0x8029B914  1440  solve1dAngLimits   [map: hkSolver__solve1dAngLimits]
//   0x8029BEB4  1160  solve1dAngularMotor   [map: hkSolver__solve1dAngularMotor]
//   0x8029C33C   664  solve1dAngular   [map: hkSolver__solve1dAngular]
//   0x8029C5D4   984  solve1dBilateral   [map: hkSolver__solve1dBilateral]
//   0x8029C9AC  1764  solve1dBilateralUserTau   [map: hkSolver__solve1dBilateralUserTau]
//   0x8029D090   984  solve1dFriction   [map: hkSolver__solve1dFriction]
//   0x8029D468  1524  solve1dLinLimits   [map: hkSolver__solve1dLinLimits]
//   0x8029DA5C  1868  solve1dLinearMotor   [map: hkSolver__solve1dLinearMotor]
//   0x8029E1A8  1204  solvePulley   [map: hkSolver__solvePulley]
//   0x8029E65C    12  next   [map: hk2Lin2AngJacobian__next]
//   0x8029E668     8  getSumLinearVel   [map: hkVelocityAccumulator__getSumLinearVel]
//   0x8029E670     8  hkAddByteOffset<20hkJacobianGotoSchema>   [map: hkJacobianGotoSchema__hkAddByteOffset_20hkJacobianGotoSchema_]
//   0x8029E678     4  storeDelayedResult   [map: hkSolver__storeDelayedResult]
//   0x8029E67C    44  getEnd   [map: hkJacobianStiffSpringChainSchema__getEnd]
//   0x8029E6A8    44  getEnd   [map: hkJacobianBallSocketChainSchema__getEnd]
//   0x8029E6D4    60  getEnd   [map: hkJacobianPoweredChainSchema__getEnd]
//   0x8029E710   416  hkSolveConstraints   [map: hkSolver__hkSolveConstraints]
//   0x8029E8B0   232  applyVelField   [map: hkSolver__applyVelField]
//   0x8029E998     4  

#pragma fp_contract on
#include <havok/hkSolver.h>
#include <havok/hkJacobianSchema.h>
#include <havok/hkJacobianHeaderSchema.h>
#include <havok/hkJacobianElement.h>
#include <havok/hkJacobianBallSocketChainSchema.h>
#include <havok/hkJacobianPoweredChainSchema.h>
#include <havok/hkJacobianStiffSpringChainSchema.h>
#include <havok/hkVelocityAccumulatorOffset.h>
#include <havok/hkVelocityAccumulator.h>
#include <havok/hk1Lin2AngJacobian.h>
#include <havok/hk2Lin2AngJacobian.h>
#include <havok/hk2AngJacobian.h>
#include <havok/hk3dAngularMotorSolverInfo.h>
#include <havok/hkChainSolverInfo.h>
#include <havok/hkVector8.h>
#include <havok/hkVector4.h>

void hkSolver::hkSolveUpload() {}

void hkSolverInfo::hkDebugPrintfAccumulators() {}

u8* hkVelocityAccumulatorOffset::getAccumulator(u8* base) const {
    return base + m_offset;
}

void hkSolver::loadVelocityAccumulators() {}

void hkSolver::popVelocityAccumulators() {}

void hkSolver::storeVelocityAccumulators() {}

u8* hkJacobianBallSocketChainSchema::getAccumulatorOffsetsBase() const {
    return (u8*)this + 0x10;
}

u8* hkJacobianBallSocketChainSchema::getMatrixBuffer(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30;
}

u8* hkJacobianBallSocketChainSchema::getTempBuffer(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30 + m_numChains * 0x90;
}

hkChainSolverInfo::hkChainSolverInfo(u32 v) {
    m_word00 = v;
}

u8* hkJacobianPoweredChainSchema::getAngularJacobians(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30;
}

u8* hkJacobianPoweredChainSchema::getAccumulatorOffsetsBase() const {
    return (u8*)this + 0x18;
}

u8* hkJacobianPoweredChainSchema::getMatrixBuffer(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30 + t * 0x20;
}

u8* hkJacobianPoweredChainSchema::getChildConstraintStatusBase() const {
    return (u8*)this + m_numChains * 4 + 0x1C;
}

u8* hkJacobianPoweredChainSchema::getTempBuffer(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30 + t * 0x20 + m_numChains * 0x3C0;
}

u8* hkJacobianPoweredChainSchema::getVelocityBuffer(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30 + t * 0x20 + m_numChains * 0x3C0 + (m_numChains + 1) * 0x20;
}

float hk2AngJacobian::getAngularRhs() const {
    return *(float*)((u8*)this + 0x1C);
}

void hk2AngJacobian::setAngularRhs(float f) {
    *(float*)((u8*)this + 0x1C) = f;
}

int hk3dAngularMotorSolverInfo::getState(int index) const {
    return (m_states >> (index << 1)) & 3;
}

void hkSolver::storeDelayedResult() {}

void hkSolver::prefetchVelocityAccumulators() {}

void hkSolver::loadFixedRegisters() {}

hkJacobianElement* hkJacobianElement::as2Ang() {
    return this;
}

u8* hkJacobianBallSocketChainSchema::getEnd(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30 + m_numChains * 0x90 + (m_numChains + 1) * 0x10;
}

u8* hkJacobianPoweredChainSchema::getEnd(u8* base) const {
    u32 t = (m_numChains << 2) - m_numChains;
    return base + t * 0x30 + t * 0x20 + m_numChains * 0x3C0 + (m_numChains + 1) * 0x20 + m_numChains * 0x20;
}

u8* hkJacobianStiffSpringChainSchema::getEnd(u8* base) const {
    u8* end = base + m_numChains * 0x30 + m_numChains * 0xC + (m_numChains + 1) * 4;
    return (u8*)(((u32)end + 0xF) & ~0xF);
}

s32 hkJacobianSchema::getSchemaType() const {
    return (s32)m_tag >> 24;
}

u32 hkJacobianSchema::getSchemaSize() const {
    return m_tag & 0xFFFF;
}

u8* hkJacobianHeaderSchema::getBodyA(u8* base) const {
    return base + unk08;
}

u8* hkJacobianHeaderSchema::getBodyB(u8* base) const {
    return base + unk0C;
}

u8* hkJacobianHeaderSchema::getJacobian(u8* base) const {
    return base + unk04;
}

hk1Lin2AngJacobian* hk1Lin2AngJacobian::next(int n) const {
    return (hk1Lin2AngJacobian*)((u8*)this + n * 0x30);
}

hk2Lin2AngJacobian* hk2Lin2AngJacobian::next(int n) const {
    return (hk2Lin2AngJacobian*)((u8*)this + n * 0x40);
}

hk2AngJacobian* hk2AngJacobian::next(int n) const {
    return (hk2AngJacobian*)((u8*)this + n * 0x20);
}

hkVector4* hkVelocityAccumulator::getSumLinearVel() {
    return (hkVector4*)((u8*)this + 0x40);
}

void hkVector8::setZero8() {
    m_v[3] = 0.0f;
    m_v[2] = 0.0f;
    m_v[1] = 0.0f;
    m_v[0] = 0.0f;
    m_v[7] = 0.0f;
    m_v[6] = 0.0f;
    m_v[5] = 0.0f;
    m_v[4] = 0.0f;
}

void hkVector8::setSub8(const hkVector8& a, const hkVector8& b) {
    m_v[0] = a.m_v[0] - b.m_v[0];
    m_v[1] = a.m_v[1] - b.m_v[1];
    m_v[2] = a.m_v[2] - b.m_v[2];
    m_v[3] = a.m_v[3] - b.m_v[3];
    m_v[4] = a.m_v[4] - b.m_v[4];
    m_v[5] = a.m_v[5] - b.m_v[5];
    m_v[6] = a.m_v[6] - b.m_v[6];
    m_v[7] = a.m_v[7] - b.m_v[7];
}

// Column-major product: this = m.col0 * v.x + m.col1 * v.y + m.col2 * v.z (columns are 0x10 apart).
void hkVector4::_setMul3(const hkRotation& m, const hkVector4& v) {
    const hkVector4* c = (const hkVector4*)&m;
    set(c[0].x * v.x + c[1].x * v.y + c[2].x * v.z, c[0].y * v.x + c[1].y * v.y + c[2].y * v.z,
        c[0].z * v.x + c[1].z * v.y + c[2].z * v.z, 0.0f);
}

void hkSolver::applyVelField(hkVector4* vel, u8* rec) {
    float zero = 0.0f;
    for (;;) {
        switch (rec[0]) {
            case 0:
                do {
                    hkVector8* z = (hkVector8*)(rec + 0x40);
                    z->m_v[3] = zero;
                    z->m_v[2] = zero;
                    z->m_v[1] = zero;
                    z->m_v[0] = zero;
                    z->m_v[7] = zero;
                    z->m_v[6] = zero;
                    z->m_v[5] = zero;
                    z->m_v[4] = zero;
                    hkVector4* acc = (hkVector4*)(rec + 0x10);
                    acc->add4(*vel);
                    rec += 0x80;
                } while (rec[0] == 0);
                break;
            case 1: {
                hkVector8* z = (hkVector8*)(rec + 0x40);
                z->m_v[3] = zero;
                z->m_v[2] = zero;
                z->m_v[1] = zero;
                z->m_v[0] = zero;
                z->m_v[7] = zero;
                z->m_v[6] = zero;
                z->m_v[5] = zero;
                z->m_v[4] = zero;
                rec += 0x80;
                break;
            }
            case 2:
                return;
            default:
                // MATCH-ONLY: the original stores through a null pointer on an invalid record type (HK_ASSERT) and loops.
                *(u32*)0 = 0;
                break;
        }
    }
}

void hkSolver::applyAngularImpulse(hkReal impulse, hkVector8* jac, hkVelocityAccumulator* a, hkVelocityAccumulator* b, hkReal* sum) {
    a->unk20.x += impulse * a->unk30.x * jac->m_v[0];
    a->unk20.y += impulse * a->unk30.y * jac->m_v[1];
    a->unk20.z += impulse * a->unk30.z * jac->m_v[2];
    b->unk20.x += impulse * b->unk30.x * jac->m_v[4];
    b->unk20.y += impulse * b->unk30.y * jac->m_v[5];
    b->unk20.z += impulse * b->unk30.z * jac->m_v[6];
    *sum += impulse;
}
