// Havok translation unit hkContinuousGsk.o (main.dol 0x80313514-0x80315780).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80313514     8  getSweptTransform   [map: hkMotionState__getSweptTransform]
//   0x8031351C     4  getRotation   [map: hkTransform__getRotation1]
//   0x80313520   628  hk4dGskCollidePointWithPlaneFinal   [map: hk4dGskVertexCollidePointsInput__hk4dGskCollidePointWithPlaneFinal]
//   0x80313794  1276  hk4dGskCollidePointsWithPlane   [map: hkContinuousGsk__hk4dGskCollidePointsWithPlane]
//   0x80313C90   804  hk4dGskCalcPointVelocityAssumingAngularVelError   [map: hkMotionState__hk4dGskCalcPointVelocityAssumingAngularVelError]
//   0x80313FB4  1312  hk4dGskCollideCalcToi   [map: hkContinuousGsk__hk4dGskCollideCalcToi]
//   0x803144D4   116  calcAngularTimInfo   [map: hkSweptTransformUtil__calcAngularTimInfo]
//   0x80314548    12  __ct   [map: hkGsk____ct]
//   0x80314554   192  init   [map: hkGsk__init]
//   0x80314614  3040  hk4dGskCalcToiFinal   [map: hkAgent3ProcessInput__hk4dGskCalcToiFinal]
//   0x803151F4   388  hk4dGskCalcWorstCaseProjectedVelocity   [map: hkMotionState__hk4dGskCalcWorstCaseProjectedVelocity]
//   0x80315378   352  hk4dGskCollPInputInitialize   [map: hkAgent3ProcessInput__hk4dGskCollPInputInitialize]
//   0x803154D8    12  __ct   [map: hk4dGskVertexCollidePointsOutput____ct]
//   0x803154E4   596  hk4dGskCalcSeparatingNormal   [map: hkTransform__hk4dGskCalcSeparatingNormal]
//   0x80315738    20  checkForChangesAndUpdateCache   [map: hkGsk__checkForChangesAndUpdateCache]
//   0x8031574C    48  hkDeallocateStack<9hkVector4>   [map: hkVector4__hkDeallocateStack_9hkVector4_]
//   0x8031577C     4  __ct   [map: hk4dGskVertexCollidePointsInput____ct]

#include <havok/hkGsk.h>
#include <havok/hk4dGskVertexCollidePointsInput.h>
#include <havok/hk4dGskVertexCollidePointsOutput.h>

hkGsk::hkGsk() {
    unk10 = 0;
}

hk4dGskVertexCollidePointsOutput::hk4dGskVertexCollidePointsOutput() {
    unk0 = 1.0f;
}

void hkGsk::checkForChangesAndUpdateCache(void* arg) {
    if (unk14 != 0) {
        exitAndExportCacheImpl(arg);
    }
}

hk4dGskVertexCollidePointsInput::hk4dGskVertexCollidePointsInput() {}

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkTransform::getRotation (bare blr), hkMotionState::getSweptTransform (+0x40)
// TODO: cleanup (shape from jj-link/brawl fn_8031351C)
extern "C" void* fn_8031351C(void* t) { return t; }
// TODO: cleanup (shape from jj-link/brawl fn_80313514)
extern "C" char* fn_80313514(char* p) { return p + 0x40; }

// fn_803144D4: body from https://github.com/jj-link/brawl src/havok/fn_803144D4.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_803144D4 {
#pragma fp_contract on

struct Data {
    char pad0[0x5c];
    float scale;
    char pad60[0x30];
    float x;
    float y;
    float z;
    float w;
};

// TODO: cleanup (shape from jj-link/brawl fn_803144D4)
extern "C" void fn_803144D4(const Data* a, const Data* b, float* outA, float* outB, float scale) {
    float sa = a->scale;
    float sb = b->scale;
    float ta = scale * sa;
    float tb = scale * sb;

    outA[0] = ta * a->x;
    outA[1] = ta * a->y;
    outA[2] = ta * a->z;
    outA[3] = ta * a->w;

    outB[0] = tb * b->x;
    outB[1] = tb * b->y;
    outB[2] = tb * b->z;
    outB[3] = tb * b->w;
}
} // namespace jj_fn_803144D4

// fn_8031574C: body from https://github.com/jj-link/brawl src/havok/fn_8031574C.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_8031574C {
struct FnObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4(u32 val);
    char pad[0x10 - 4];
    u32 m10;
    char pad2[4];
    u32 m18;
};


// TODO: cleanup (shape from jj-link/brawl fn_8031574C)
extern "C" void fn_8031574C(u32 val) {
    FnObj* obj = ((FnObj*)hkThreadMemory::s_instance);
    obj->m10 = val;
    if (val == obj->m18) {
        obj->v4(val);
    }
}
} // namespace jj_fn_8031574C

// fn_803154E4: body from https://github.com/jj-link/brawl src/havok/fn_803154E4.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
#include <havok/hkTransform.h>
namespace jj_fn_803154E4 {
#pragma fp_contract on

struct Vec4 {
    float x, y, z, w;
};

struct Matrix {
    float m[4][4];
} __attribute__((aligned(16)));

struct State {
    int a, b;
    char pad[0x18];
    Vec4 p;
    char pad30[0x70];
    Vec4 q;
    char padb0[0x80];
    Vec4 r;
};

extern "C" void fn_803198C8(State*, Vec4*, Vec4*, Matrix*, Vec4*);
extern "C" const float lbl_805A4650;

// TODO: cleanup (shape from jj-link/brawl fn_803154E4)
extern "C" void fn_803154E4(
    Matrix* a, Matrix* b, Vec4* sa, Vec4* sb, State* s, Vec4* out, Vec4* out2)
{
    Matrix m;
    Vec4 v;
    Vec4 p;

    ((hkTransform*)(&m))->setMulInverseMul(*(const hkTransform*)(a), *(const hkTransform*)(b));
    fn_803198C8(s, sa, sb, &m, &v);

    float x = v.x, y = v.y, z = v.z;
    float ox = z * a->m[2][0] + (x * a->m[0][0] + y * a->m[1][0]);
    float oy = z * a->m[2][1] + (x * a->m[0][1] + y * a->m[1][1]);
    float oz = z * a->m[2][2] + (x * a->m[0][2] + y * a->m[1][2]);
    out->x = ox;
    out->y = oy;
    out->z = oz;
    out->w = lbl_805A4650;

    if (s->a == 1) {
        p = s->p;
    } else if (s->b == 1) {
        float qz, qy, qx;
        qx = s->q.x;
        qy = s->q.y;
        qz = s->q.z;
        float qw = s->q.w;
        float w = v.w;
        float px = w * v.x + qx;
        float py = w * v.y + qy;
        float pz = w * v.z + qz;
        p.w = w * w + qw;
        p.x = px;
        p.y = py;
        p.z = pz;
    } else {
        p = s->r;
    }

    p.x = p.x - v.w * v.x;
    p.y = p.y - v.w * v.y;
    p.z = p.z - v.w * v.z;
    p.w = p.w - v.w * v.w;

    float dz = p.z - m.m[3][2];
    float dy = p.y - m.m[3][1];
    float dx = p.x - m.m[3][0];

    out2->x = dz * m.m[0][2] + (dx * m.m[0][0] + dy * m.m[0][1]);
    out2->y = dz * m.m[1][2] + (dx * m.m[1][0] + dy * m.m[1][1]);
    out2->z = dz * m.m[2][2] + (dx * m.m[2][0] + dy * m.m[2][1]);
    out2->w = lbl_805A4650;

    float ra = sa->w;
    float rb = sb->w;
    out->w = (v.w - ra) - rb;
}
} // namespace jj_fn_803154E4
