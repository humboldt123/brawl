// Havok translation unit hkGenericConstraintData.o (main.dol 0x802DBC80-0x802DD570).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DBC80    52  finishLoadedObjecthkGenericConstraintData   [map: hkGenericConstraintData__finishLoadedObjecthkGenericConstraintData]
//   0x802DBCB4    20  cleanupLoadedObjecthkGenericConstraintData   [map: hkGenericConstraintData__cleanupLoadedObjecthkGenericConstraintData]
//   0x802DBCC8    72  getVtablehkGenericConstraintData   [map: hkGenericConstraintData__getVtablehkGenericConstraintData]
//   0x802DBD10    88  __ct   [map: hkGenericConstraintData____ct]
//   0x802DBD68   416  __dt   [map: hkGenericConstraintData____dt]
//   0x802DBF08    76  getConstraintInfo   [map: hkGenericConstraintData__getConstraintInfo]
//   0x802DBF54    20  getRuntimeInfo   [map: hkGenericConstraintData__getRuntimeInfo]
//   0x802DBF68   104  buildJacobian   [map: hkGenericConstraintData__buildJacobian]
//   0x802DBFD0   260  constrainAllLinearW   [map: hkGenericConstraintData__constrainAllLinearW]
//   0x802DC0D4   112  calcDeltaAngleAroundAxis   [map: hkGenericConstraintDataParameters__calcDeltaAngleAroundAxis]
//   0x802DC144   248  setLinearFrictionW   [map: hkGenericConstraintData__setLinearFrictionW]
//   0x802DC23C  1212  hatchScheme   [map: hkGenericConstraintData__hatchScheme]
//   0x802DC6F8   108  _setRotatedDir   [map: hkVector4___setRotatedDir]
//   0x802DC764   208  constrainLinearW   [map: hkGenericConstraintData__constrainLinearW]
//   0x802DC834   100  __as   [map: hkRotation____as]
//   0x802DC898   404  constrainToAngularW   [map: hkGenericConstraintData__constrainToAngularW]
//   0x802DCA2C   428  constrainAllAngularW   [map: hkGenericConstraintData__constrainAllAngularW]
//   0x802DCBD8   240  setLinearLimitW   [map: hkGenericConstraintData__setLinearLimitW]
//   0x802DCCC8   344  setAngularLimitW   [map: hkGenericConstraintData__setAngularLimitW]
//   0x802DCE20   452  setConeLimitW   [map: hkGenericConstraintData__setConeLimitW]
//   0x802DCFE4   228  setTwistLimitW   [map: hkGenericConstraintData__setTwistLimitW]
//   0x802DD0C8   512  setAngularMotorW   [map: hkGenericConstraintData__setAngularMotorW]
//   0x802DD2C8   404  setLinearMotorW   [map: hkGenericConstraintData__setLinearMotorW]
//   0x802DD45C   152  setAngularFrictionW   [map: hkGenericConstraintData__setAngularFrictionW]
//   0x802DD4F4     4  __ct   [map: hkGenericConstraintDataParameters____ct]
//   0x802DD4F8     8  begin   [map: hkArray_i___begin]
//   0x802DD500     8  begin   [map: hkArray_9hkVector4___begin]
//   0x802DD508     8  begin   [map: hkArray_P20hkConstraintModifier___begin]
//   0x802DD510     8  isValid   [map: hkGenericConstraintData__isValid]
//   0x802DD518     8  getType   [map: hkGenericConstraintData__getType]
//   0x802DD520    80  __sinit_\hkGenericConstraintData_cpp   [map: hkGenericConstraintDatacpp____sinit_]

#include <havok/hkGenericConstraintData.h>
#include <havok/hkRegistry.h>
#include <havok/hkMath.h>

// Stand-in for a helper in another TU (not yet named).
extern "C" void fn_80288B40(void* a, void* b, u32 c, u32 d);

void hkGenericConstraintData::finishLoadedObjecthkGenericConstraintData(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkGenericConstraintData(flag);
}

void hkGenericConstraintData::cleanupLoadedObjecthkGenericConstraintData(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

#pragma push
#pragma auto_inline off
const void* hkGenericConstraintData::getVtablehkGenericConstraintData() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x58] __attribute__((aligned(16)));
    ::new (buf) hkGenericConstraintData(flag);
    return *(const void**)buf;
}
#pragma pop

hkGenericConstraintData::hkGenericConstraintData(hkFinishLoadedObjectFlag flag)
    : hkConstraintData(flag), m_scheme(flag) {
    m_atoms.m_bridgeAtom.init(m_atoms.m_bridgeAtom.m_constraintData);
    m_atoms.m_bridgeAtom.init(this);
}

// Releases the motors (reference counted), then the member arrays are destroyed in reverse order.
hkGenericConstraintData::~hkGenericConstraintData() {
    for (int i = 0; i < m_scheme.m_motors.m_size; i++) {
        ((hkReferencedObject**)m_scheme.m_motors.m_data)[i]->removeReference();
    }
}

void hkGenericConstraintData::getConstraintInfo(hkConstraintInfo* info) {
    ((hkConstraintInfoWithAtoms*)info)->unk10 = &m_atoms.m_bridgeAtom;
    ((hkConstraintInfoWithAtoms*)info)->unk14 = (u32)((u8*)&m_scheme - (u8*)&m_atoms.m_bridgeAtom);
    info->m_maxSizeOfJacobians = 0;
    info->m_sizeOfJacobians = 0;
    info->m_sizeOfSchemas = 0;
    info->m_numSolverResults = 0;
    info->m_maxSizeOfJacobians = m_scheme.m_info.m_maxSizeOfJacobians;
    info->m_sizeOfJacobians = m_scheme.m_info.m_sizeOfJacobians;
    info->m_sizeOfSchemas = m_scheme.m_info.m_sizeOfSchemas;
    info->m_numSolverResults = m_scheme.m_info.m_numSolverResults;
}

void hkGenericConstraintData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) {
    u32 n = m_scheme.m_info.m_numSolverResults;
    out->unk04 = n;
    out->unk00 = n * 8;
}

void hkGenericConstraintData::buildJacobian(void* a, void* b) {
    fn_80288B40(a, b, *(u32*)((u8*)a + 0x44), 8);
    hatchScheme(&m_scheme, a, b);
}

// this = R * v (rotation columns are 0x10 apart; the w lane is the constant 0). Same shape as hkSolver's _setMul3.
void hkVector4::setRotatedDir(const hkRotation& r, const hkVector4& v) {
    const hkVector4* c = (const hkVector4*)&r;
    set(c[0].x * v.x + c[1].x * v.y + c[2].x * v.z, c[0].y * v.x + c[1].y * v.y + c[2].y * v.z,
        c[0].z * v.x + c[1].z * v.y + c[2].z * v.z, 0.0f);
}

// HYPOTHESIS: the two axis indices come from a lookup table (values unknown); the result is atan2 of two dot products.
#pragma push
#pragma auto_inline off // MATCH-ONLY: kept out of line for fn_802DCCC8, which calls it
float hkGenericConstraintDataParameters::calcDeltaAngleAroundAxis(int axis, const u8* base) {
    static const int s_axisTable[] = {1, 2, 0, 1, 2};
    const u8* e1 = base + s_axisTable[axis + 1] * 0x10;
    const u8* e2 = base + s_axisTable[axis + 2] * 0x10;
    float a = *(float*)(e2 + 0x84) * *(float*)(e1 + 0x54) + *(float*)(e2 + 0x80) * *(float*)(e1 + 0x50) +
              *(float*)(e2 + 0x88) * *(float*)(e1 + 0x58);
    float b = *(float*)(e1 + 0x84) * *(float*)(e1 + 0x54) + *(float*)(e1 + 0x80) * *(float*)(e1 + 0x50) +
              *(float*)(e1 + 0x88) * *(float*)(e1 + 0x58);
    return hkMathTypes::hkMath_atan2fApproximation(a, b);
}
#pragma pop

hkGenericConstraintDataParameters::hkGenericConstraintDataParameters() {
}

hkBool hkGenericConstraintData::isValid() const {
    return hkBool(true);
}

int hkGenericConstraintData::getType() const {
    return 10;
}

static hkStaticTypeInfo hkGenericConstraintDataTypeInfo("hkGenericConstraintData", hkGenericConstraintData::finishLoadedObjecthkGenericConstraintData, hkGenericConstraintData::cleanupLoadedObjecthkGenericConstraintData,
                                      hkGenericConstraintData::getVtablehkGenericConstraintData());

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkArray<int>::begin, hkArray<hkVector4>::begin, hkArray<hkConstraintModifier*>::begin
// TODO: cleanup (shape from jj-link/brawl fn_802DD4F8)
extern "C" void* fn_802DD4F8(void* const* p) { return *p; }
// TODO: cleanup (shape from jj-link/brawl fn_802DD500)
extern "C" void* fn_802DD500(void* const* p) { return *p; }
// TODO: cleanup (shape from jj-link/brawl fn_802DD508)
extern "C" void* fn_802DD508(void* const* p) { return *p; }

// Shape from https://github.com/jj-link/brawl src/havok/fn_802DC834.cpp.
// hkRotation::operator=
// TODO: cleanup (shape from jj-link/brawl fn_802DC834)
extern "C" void fn_802DC834(float* d, const float* s) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
    d[4] = s[4];
    d[5] = s[5];
    d[6] = s[6];
    d[7] = s[7];
    d[8] = s[8];
    d[9] = s[9];
    d[10] = s[10];
    d[11] = s[11];
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_802DC6F8.cpp.
// hkVector4::_setRotatedDir(const hkRotation&, const hkVector4&)
// TODO: cleanup (shape from jj-link/brawl fn_802DC6F8)
extern "C" void fn_802DC6F8(float* out, const float* m, const float* v) {
    float a, b, c;
    float y = v[1];
    float z = v[2];
    float x = v[0];
    a = (z * m[8]) + ((x * m[0]) + (y * m[4]));
    b = (z * m[9]) + ((x * m[1]) + (y * m[5]));
    c = (z * m[10]) + ((x * m[2]) + (y * m[6]));
    out[0] = a;
    out[1] = b;
    out[2] = c;
    out[3] = 0.0f;
}

// fn_802DC764: body from https://github.com/jj-link/brawl src/havok/fn_802DC764.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DC764 {
struct Context {
    float values[8];
    float table[0x98 / 4];
    int count;
};

struct Work {
    float values[12];
};

extern "C" {
void fn_8028E4A4(void*, int, int);
}

// TODO: cleanup (shape from jj-link/brawl fn_802DC764)
extern "C" void fn_802DC764(void* a, void** cur, int b, int c, Context* ctx, int d, int e) {
    Work work __attribute__((aligned(16)));
    void* t;
    int idx;
    float* row;

    t = *cur;
    *cur = (char*)t + 4;
    idx = *(int*)*cur;

    row = (float*)((char*)ctx + idx * 16);
    work.values[8] = row[8];
    work.values[9] = row[9];
    work.values[10] = row[10];
    work.values[11] = row[11];

    work.values[0] = ctx->values[0];
    work.values[1] = ctx->values[1];
    work.values[2] = ctx->values[2];
    work.values[3] = ctx->values[3];
    work.values[4] = ctx->values[4];
    work.values[5] = ctx->values[5];
    work.values[6] = ctx->values[6];
    work.values[7] = ctx->values[7];

    fn_8028E4A4(&work, d, e);
    ctx->count += 1;
}
} // namespace jj_fn_802DC764

// fn_802DCBD8: body from https://github.com/jj-link/brawl src/havok/fn_802DCBD8.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DCBD8 {
struct Context {
    float values[8];
    float table[0x98 / 4];
    int count;
};

struct Work {
    float a[8];
    float b[4];
    float c[2];
};

extern "C" void fn_8028ED40(Work* w, int a, int b, void* d, void* e);

// TODO: cleanup (shape from jj-link/brawl fn_802DCBD8)
extern "C" void fn_802DCBD8(void* a, void** arg1, void** arg2, void* unused, Context* ctx, int arg5, int arg6) {
    Work work __attribute__((aligned(16)));
    void* t;
    void* n;
    void* t2;
    int idx;
    float* row;

    t = *arg1;
    n = (char*)t + 4;
    *arg1 = n;
    idx = *(int*)n;

    row = (float*)((char*)ctx + idx * 16);
    work.b[0] = row[8];
    work.b[1] = row[9];
    work.b[2] = row[10];
    work.b[3] = row[11];

    work.a[0] = ctx->values[0];
    work.a[1] = ctx->values[1];
    work.a[2] = ctx->values[2];
    work.a[3] = ctx->values[3];
    work.a[4] = ctx->values[4];
    work.a[5] = ctx->values[5];
    work.a[6] = ctx->values[6];
    work.a[7] = ctx->values[7];

    t2 = *arg2;
    *arg2 = (char*)t2 + 0x10;
    work.c[0] = ((float*)t2)[0];
    work.c[1] = ((float*)t2)[1];

    fn_8028ED40(&work, arg5, arg6, t2, ctx);
    ctx->count += 1;
}
} // namespace jj_fn_802DCBD8

// fn_802DBFD0: body from https://github.com/jj-link/brawl src/havok/fn_802DBFD0.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DBFD0 {
extern "C" {
extern "C" const float lbl_805A4270;
extern "C" const float lbl_805A4274;
void fn_8028E4A4(void*, int, int);
}

struct Context {
    float values[8];
    char pad[0x98];
    int count;
};

struct Work {
    float values[12];
};

// TODO: cleanup (shape from jj-link/brawl fn_802DBFD0)
extern "C" void fn_802DBFD0(
    void*, void*, void*, void*, Context* ctx, int a, int b)
{
    Work work __attribute__((aligned(16)));

    work.values[0] = ctx->values[0];
    work.values[1] = ctx->values[1];
    work.values[2] = ctx->values[2];
    work.values[3] = ctx->values[3];
    work.values[4] = ctx->values[4];
    work.values[5] = ctx->values[5];
    work.values[6] = ctx->values[6];
    work.values[7] = ctx->values[7];

    work.values[11] = lbl_805A4270;
    work.values[10] = lbl_805A4270;
    work.values[9] = lbl_805A4270;
    work.values[8] = lbl_805A4274;
    fn_8028E4A4(&work, a, b);

    work.values[8] = lbl_805A4270;
    work.values[9] = lbl_805A4274;
    fn_8028E4A4(&work, a, b);

    work.values[9] = lbl_805A4270;
    work.values[10] = lbl_805A4274;
    fn_8028E4A4(&work, a, b);

    ctx->count += 3;
}
} // namespace jj_fn_802DBFD0

// fn_802DC898: body from https://github.com/jj-link/brawl src/havok/fn_802DC898.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DC898 {
extern "C" {
extern "C" int lbl_80487A68[];
void fn_8028D8C0(float*, int, int, void*, void*, void*, int, void*);
}
// TODO: cleanup (shape from jj-link/brawl fn_802DC898)
extern "C" void fn_802DC898(void*, int** p, void*, void*, char* base, int i, int j) {
    float v[12] __attribute__((aligned(16)));
    ++*p;
    int k = **p;
    int a = lbl_80487A68[k + 1];
    int b = lbl_80487A68[k + 2];
    char* r6 = base + k * 0x10;
    char* r7 = base + b * 0x10;
    char* r8 = base + a * 0x10;
    v[4] = *(float*)(r8 + 0x50);
    v[5] = *(float*)(r8 + 0x54);
    v[6] = *(float*)(r8 + 0x58);
    v[7] = *(float*)(r8 + 0x5C);
    v[0] = *(float*)(r7 + 0x50);
    v[1] = *(float*)(r7 + 0x54);
    v[2] = *(float*)(r7 + 0x58);
    v[3] = *(float*)(r7 + 0x5C);
    v[8] = *(float*)(r6 + 0x80);
    v[9] = *(float*)(r6 + 0x84);
    v[10] = *(float*)(r6 + 0x88);
    v[11] = *(float*)(r6 + 0x8C);
    fn_8028D8C0(v, i, j, r6, r7, r8, b, &lbl_80487A68);
    float t[4] __attribute__((aligned(16)));
    t[0] = -v[4];
    t[1] = -v[5];
    t[2] = -v[6];
    t[3] = -v[7];
    v[4] = v[0]; v[5] = v[1]; v[6] = v[2]; v[7] = v[3];
    v[0] = t[0]; v[1] = t[1]; v[2] = t[2]; v[3] = t[3];
    ((void (*)(float*, int, int))fn_8028D8C0)(v, i, j);
    *(int*)(base + 0xB8) += 2;
}
} // namespace jj_fn_802DC898

// fn_802DCA2C: body from https://github.com/jj-link/brawl src/havok/fn_802DCA2C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802DCA2C {
struct Vec { float x,y,z,w; };
struct Context {
    char pad[0x50];
    Vec a,b,c,d,e,f;
    char pad2[8];
    int count;
};
struct Work { Vec a,b,c; };
extern "C" void fn_8028D8C0(Work*, int, int);
// TODO: cleanup (shape from jj-link/brawl fn_802DCA2C)
extern "C" void fn_802DCA2C(void*, void*, void*, void*, Context* ctx, int a, int b) {
    Work first __attribute__((aligned(16)));
    Work second __attribute__((aligned(16)));
    first.a = ctx->a;
    first.c = ctx->e;
    first.b = ctx->c;
    fn_8028D8C0(&first, a, b);
    second.a = ctx->b;
    second.c = ctx->f;
    second.b = first.a;
    fn_8028D8C0(&second, a, b);
    second.c = ctx->d;
    second.b = second.a;
    second.a = first.b;
    fn_8028D8C0(&second, a, b);
    ctx->count += 3;
}
} // namespace jj_fn_802DCA2C

// TODO: cleanup (shape from jj-link/brawl fn_802DCCC8)
// fn_802DCCC8: body from https://github.com/jj-link/brawl src/havok/fn_802DCCC8.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkGenericConstraintData.h>
namespace jj_fn_802DCCC8 {
struct Vec { float x,y,z,w; };
struct Context { char pad[0x50]; Vec values[6]; char pad2[8]; int count; };
struct Work { Vec a,b; };
extern "C" {
extern const float lbl_805A4270, lbl_805A4278, lbl_805A427C, lbl_805A4280;
void fn_80287FE8(void*, void*, void*);
void fn_802DCCC8(void*, int** indices, Vec** data, void*, Context* ctx, void* a, void* b) {
    Work work __attribute__((aligned(16)));
    ++*indices;
    int index = **indices;
    work.b.w = lbl_805A4278;
    work.a = ctx->values[index];
    work.b.z = hkGenericConstraintDataParameters::calcDeltaAngleAroundAxis(index, (const u8*)(ctx));
    Vec* v = *data;
    ++*data;
    work.b.x = v->x;
    work.b.y = v->y;
    if (work.b.z < lbl_805A4270 && v->z > lbl_805A4270 && v->z - work.b.z > lbl_805A427C)
        work.b.z += lbl_805A4280;
    if (work.b.z > lbl_805A4270 && v->z < lbl_805A4270 && work.b.z - v->z > lbl_805A427C)
        work.b.z -= lbl_805A4280;
    v->z = work.b.z;
    fn_80287FE8(&work, a, b);
    ctx->count++;
}
}
} // namespace jj_fn_802DCCC8
