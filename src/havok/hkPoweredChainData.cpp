// Havok translation unit hkPoweredChainData.o (main.dol 0x802DAD74-0x802DB990).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DAD74     8  getType   [map: hkPoweredChainData__getType]
//   0x802DAD7C    36  __as   [map: hkQuaternion____as]
//   0x802DADA0   116  getConstraintInfo   [map: hkPoweredChainData__getConstraintInfo]
//   0x802DAE14    44  getRuntimeInfo   [map: hkPoweredChainData__getRuntimeInfo]
//   0x802DAE40    20  getConstraintFlags   [map: hkPoweredChainData__getConstraintFlags]
//   0x802DAE54    12  getRuntime   [map: hkConstraintInstance__getRuntime]
//   0x802DAE60  1788  buildJacobian   [map: hkPoweredChainData__buildJacobian]
//   0x802DB55C     8  getEntityA   [map: hkConstraintInstance__getEntityA]
//   0x802DB564     8  hkAddByteOffset<21hkVelocityAccumulator>   [map: hkVelocityAccumulator__hkAddByteOffset_21hkVelocityAccumulator_]
//   0x802DB56C     8  getSize   [map: hkArray_P8hkEntity___getSize]
//   0x802DB574     8  getMotion   [map: hkEntity__getMotion]
//   0x802DB57C    12  __ct   [map: hkVelocityAccumulatorOffset____ct]
//   0x802DB588     8  getTransform   [map: hkMotion__getTransform]
//   0x802DB590     8  val   [map: hkPadSpu_P17hkJacobianElement___val]
//   0x802DB598     4  __ct   [map: hkVector4____ct]
//   0x802DB59C   132  _setTransformedPos   [map: hkVector4___setTransformedPos]
//   0x802DB620     4  __ct   [map: hkQuaternion____ct]
//   0x802DB624     4  getRotation   [map: hkTransform__getRotation]
//   0x802DB628   168  setMul   [map: hkQuaternion__setMul]
//   0x802DB6D0    20  set   [map: hkVector4__set]
//   0x802DB6E4    32  getMotorRuntimeQuaternions   [map: hkPoweredChainData__getMotorRuntimeQuaternions]
//   0x802DB704    36  lengthSquared4   [map: hkVector4__lengthSquared4]
//   0x802DB728   220  estimateAngleToLs   [map: hkQuaternion__estimateAngleToLs]
//   0x802DB804    68  sub4   [map: hkVector4__sub4]
//   0x802DB848    48  setNeg3   [map: hkVector4__setNeg3]
//   0x802DB878     4  __ct   [map: hkRotation____ct]
//   0x802DB87C    36  __ct   [map: hkVector4____ct1]
//   0x802DB8A0    12  getColumn   [map: hkMatrix3__getColumn]
//   0x802DB8AC     8  getSolverResults   [map: hkPoweredChainData__getSolverResults]
//   0x802DB8B4    92  hk1dAngularVelocityMotorCommitJacobianInMotorInfo   [map: hk1dConstraintMotorInfo__hk1dAngularVelocityMotorCommitJacobianInMotorInfo]
//   0x802DB910   108  swap<12hkQuaternion>   [map: hkAlgorithm__swap_12hkQuaternion_]
//   0x802DB97C     8  __ct   [map: hkPadSpuLong_P20hkConstraintInstance_____ct]
//   0x802DB984    12  __as   [map: hkPadSpuLong_P20hkConstraintInstance_____as]

#include <havok/hkPoweredChainData.h>

u32 hkPoweredChainData::getType() const {
    return 102;
}

// Info block: the sizes are linear in the chain length n.
void hkPoweredChainData::getConstraintInfo(hkConstraintInfo* info) const {
    info->unk10 = (void*)&m_bridgeAtom;
    info->unk14 = (u32)((u8*)&m_unk18 - (u8*)&m_bridgeAtom);
    info->unk00 = 0;
    info->unk04 = 0;
    info->unk0C = 0;
    info->unk08 = 0x18;
    s32 n = m_chainLength;
    s32 n1 = n + 1;
    s32 sizeA = (n1 << 5) + (n << 5);
    s32 sizeB = (n1 << 2) + n * 0x4C;
    info->unk0C = n * 6;
    info->unk08 = sizeB + 0x30;
    s32 sizeC = n * 0xF0;
    s32 sizeD = n * 0x3C0;
    info->unk04 = sizeA + (sizeC + sizeD);
}

void hkPoweredChainData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) const {
    s32 n = m_chainLength;
    s32 a = n * 6;
    out->unk04 = a;
    out->unk00 = (n << 4) + (((n + 3) & ~3) + (a << 3));
}

void* hkPoweredChainData::getConstraintFlags(void* base) const {
    return (u8*)base + m_chainLength * 48;
}
