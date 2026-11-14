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
#include <types.h>

int hkPoweredChainData::getType() const {
    return 102;
}

// Info block: the sizes are linear in the chain length n.
void hkPoweredChainData::getConstraintInfo(hkConstraintInfo* info) const {
    ((hkConstraintInfoWithAtoms*)info)->unk10 = (void*)&m_bridgeAtom;
    ((hkConstraintInfoWithAtoms*)info)->unk14 = (u32)((u8*)&m_unk18 - (u8*)&m_bridgeAtom);
    info->m_maxSizeOfJacobians = 0;
    info->m_sizeOfJacobians = 0;
    info->m_numSolverResults = 0;
    info->m_sizeOfSchemas = 0x18;
    s32 n = m_chainLength;
    s32 n1 = n + 1;
    s32 sizeA = n1 * 0x20 + n * 0x20;
    s32 sizeB = (n1 << 2) + n * 0x4C;
    info->m_numSolverResults = n * 6;
    info->m_sizeOfSchemas = sizeB + 0x30;
    s32 sizeC = n * 0xF0;
    s32 sizeD = n * 0x3C0;
    info->m_sizeOfJacobians = sizeA + (sizeC + sizeD);
}

void hkPoweredChainData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) const {
    s32 n = m_chainLength;
    s32 a = n * 6;
    out->unk04 = a;
    s32 b = ((n + 3) & ~3) + a * 8;
    out->unk00 = b + n * 16;
}

void* hkPoweredChainData::getConstraintFlags(void* base) const {
    return (u8*)base + ((m_chainLength * 6) << 3);
}

void* hkPoweredChainData::getMotorRuntimeQuaternions(void* base) const {
    s32 n = m_chainLength;
    s32 a = n * 6;
    return (((n + 3) & ~3)) + ((u8*)base + (a << 3));
}

// HYPOTHESIS: returns its argument unchanged (the target is a bare `mr r3, r4`).
void* hkPoweredChainData::getSolverResults(void* results) const {
    return results;
}

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" int fn_802DB564(int arg0, int arg1) {
    return arg0 + arg1;
}

extern "C" void fn_802DB57C(int* arg0, int arg1, int arg2) {
    *arg0 = arg2 - arg1;
}

extern "C" void fn_802DB984(int* destination, const int* source)
{
    *destination = *source;
}

// hkTransform::getRotation / hkRotation ctor (bare blr)
// TODO: cleanup (shape from jj-link/brawl fn_802DB624)
extern "C" void* fn_802DB624(void* t) { return t; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB878)
extern "C" void fn_802DB878(void*) {}
// hkConstraintInstance::getEntityA, hkArray<hkEntity*>::getSize, hkEntity::getMotion, hkMotion::getTransform,
// hkPadSpu<hkJacobianElement*>::val, hkPadSpuLong<hkConstraintInstance*> ctor
// TODO: cleanup (shape from jj-link/brawl fn_802DB55C)
extern "C" void* fn_802DB55C(const char* p) { return *(void* const*)(p + 0x14); }
// TODO: cleanup (shape from jj-link/brawl fn_802DB56C)
extern "C" int fn_802DB56C(const char* p) { return *(const int*)(p + 0x4); }
// TODO: cleanup (shape from jj-link/brawl fn_802DB574)
extern "C" char* fn_802DB574(char* p) { return p + 0xA0; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB588)
extern "C" char* fn_802DB588(char* p) { return p + 0x10; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB590)
extern "C" void* fn_802DB590(void* const* p) { return *p; }
// TODO: cleanup (shape from jj-link/brawl fn_802DB97C)
extern "C" void fn_802DB97C(void** p, void* v) { *p = v; }
// hkConstraintInstance::getRuntime (m_internal->m_runtime), hkMatrix3::getColumn
// TODO: cleanup (shape from jj-link/brawl fn_802DAE54)
extern "C" void* fn_802DAE54(const char* p) { return *(void* const*)(*(const char* const*)(p + 0x28) + 0x20); }
// TODO: cleanup (shape from jj-link/brawl fn_802DB8A0)
extern "C" char* fn_802DB8A0(char* p, int i) { return p + i * 16; }
// hkVector4::set, hkQuaternion::operator=, hkVector4::lengthSquared4, setNeg3, sub4
// TODO: cleanup (shape from jj-link/brawl fn_802DB6D0)
extern "C" void fn_802DB6D0(float* p, float a, float b, float c, float d) {
    p[0] = a;
    p[1] = b;
    p[2] = c;
    p[3] = d;
}
// TODO: cleanup (shape from jj-link/brawl fn_802DAD7C)
extern "C" void fn_802DAD7C(float* d, const float* s) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
}
// TODO: cleanup (shape from jj-link/brawl fn_802DB704)
extern "C" float fn_802DB704(const float* v) {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
}
// TODO: cleanup (shape from jj-link/brawl fn_802DB848)
extern "C" void fn_802DB848(float* d, const float* s) {
    d[0] = -s[0];
    d[1] = -s[1];
    d[2] = -s[2];
    d[3] = s[3];
}
// TODO: cleanup (shape from jj-link/brawl fn_802DB804)
extern "C" void fn_802DB804(float* a, const float* b) {
    a[0] = a[0] - b[0];
    a[1] = a[1] - b[1];
    a[2] = a[2] - b[2];
    a[3] = a[3] - b[3];
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_802DB910.cpp.
// hkAlgorithm::swap<hkQuaternion>
struct hkFloat4A_802DB910 {
    float x, y, z, w;
} __attribute__((aligned(16)));
// TODO: cleanup (shape from jj-link/brawl fn_802DB910)
extern "C" void fn_802DB910(hkFloat4A_802DB910* a, hkFloat4A_802DB910* b) {
    hkFloat4A_802DB910 temp;
    temp.x = a->x;
    temp.y = a->y;
    temp.z = a->z;
    temp.w = a->w;
    *a = *b;
    *b = temp;
}


// fn_802DB8B4: body from https://github.com/jj-link/brawl src/havok/fn_802DB8B4.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DB8B4 {
// TODO: cleanup (shape from jj-link/brawl fn_802DB8B4)
extern "C" void fn_802DB8B4(float* a, const float* b, float** c, float* d) {
    const float s = b[1];
    const float s2 = b[2];
    float* const p = *c;
    d[1] = a[2] * s;
    d[2] = a[3] * s;
    d[3] = a[1];
    d[4] = a[4];
    d[5] = a[5];
    d[0] = 1.0f;
    p[7] = a[0] * s2;
    *c = p + 8;
}
} // namespace jj_fn_802DB8B4

// fn_802DB59C: body from https://github.com/jj-link/brawl src/havok/fn_802DB59C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DB59C {
#pragma fp_contract on

extern "C" {

extern "C" const float lbl_805A426C;

// TODO: cleanup (shape from jj-link/brawl fn_802DB59C)
void fn_802DB59C(float *out, float *m, float *v)
{
    float a, b, c;
    float x = v[0];
    float y = v[1];
    float z = v[2];

    a = m[12] + ((z * m[8]) + ((x * m[0]) + (y * m[4])));
    b = m[13] + ((z * m[9]) + ((x * m[1]) + (y * m[5])));
    c = m[14] + ((z * m[10]) + ((x * m[2]) + (y * m[6])));

    out[3] = lbl_805A426C;
    out[0] = a;
    out[1] = b;
    out[2] = c;
}

}
} // namespace jj_fn_802DB59C
