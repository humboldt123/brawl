// Havok translation unit hkGskBaseAgent.o (main.dol 0x802B1938-0x802B2870).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B1938     4  processCollision   [map: hkGskBaseAgent__processCollision]
//   0x802B193C   304  __ct   [map: hkGskBaseAgent____ct]
//   0x802B1A6C    32  invalidateTim   [map: hkGskBaseAgent__invalidateTim]
//   0x802B1A8C    32  cleanup   [map: hkGskBaseAgent__cleanup]
//   0x802B1AAC    92  __dt   [map: hkGskBaseAgent____dt]
//   0x802B1B08    52  warpTime   [map: hkGskBaseAgent__warpTime]
//   0x802B1B3C   256  calcSeparatingNormal   [map: hkGskBaseAgent__calcSeparatingNormal]
//   0x802B1C3C  1284  staticGetClosestPoints   [map: hkGskBaseAgent__staticGetClosestPoints]
//   0x802B2140   848  getPenetrations   [map: hkGskBaseAgent__getPenetrations]
//   0x802B2490   568  staticGetPenetrations   [map: hkGskBaseAgent__staticGetPenetrations]
//   0x802B26C8   424  getClosestPoints   [map: hkGskBaseAgent__getClosestPoints]

#include <havok/hkGskBaseAgent.h>
#include <havok/hkCollidable.h>
#include <havok/hkShape.h>
#include <havok/hkShapeType.h>
#include <havok/hkTransform.h>

// Stand-ins for the GSK setup helpers (other units, not recovered yet). fn_8031C8CC / fn_8031CD64 initialise the
// sub-object at 0x0C for a triangle / other shape pair.
extern "C" void fn_8031C8CC(void* self, void* shapeA, void* shapeB, void* relative);
extern "C" void fn_8031CD64(void* self, void* shapeA, void* shapeB, void* relative);

hkGskBaseAgent::hkGskBaseAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkContactMgr* contactMgr) : hkIterativeLinearCastAgent((int)contactMgr) {
    hkTransform relative;
    hkShape* shapeA = bodyA.m_shape;
    hkShape* shapeB = bodyB.m_shape;
    // a motion state starts with its hkTransform, so the pointers can be used as transforms directly
    const hkTransform& tb = *(const hkTransform*)bodyB.m_motion;
    const hkTransform& ta = *(const hkTransform*)bodyA.m_motion;
    relative.setMulInverseMul(ta, tb);
    if (shapeB->getType() == HK_SHAPE_TRIANGLE) {
        fn_8031C8CC(unkC, shapeA, shapeB, &relative);
    } else {
        fn_8031CD64(unkC, shapeA, shapeB, &relative);
    }
    unk28 = 0.0f;
    unk24 = 0.0f;
    unk20 = 0.0f;
    unk2C = -1.0f;
    unk18 = -1.0f;
    // BYTES: the depths are read into hkSimdReal locals (propagation off, as in the target), each right after its walk.
    hkSimdReal valueA = ((const hkCollidable*)bodyA.getRootCollidable())->m_allowedPenetrationDepth;
    hkSimdReal valueB = ((const hkCollidable*)bodyB.getRootCollidable())->m_allowedPenetrationDepth;
    unk1C = valueA < valueB ? valueA : valueB;
}

void hkGskBaseAgent::processCollision(const hkCdBody& unk0, const hkCdBody& unk1, hkProcessCollisionInput& unk2, void* unk3) {}

void hkGskBaseAgent::invalidateTim(void* arg) {
    unk2C = 0.0f;
    unk28 = 0.0f;
    unk24 = 0.0f;
    unk20 = 0.0f;
    unk18 = -1.0f;
}

void hkGskBaseAgent::cleanup() {
    delete this;
}

void hkGskBaseAgent::warpTime(float oldTime, float newTime, void* arg) {
    if (unk18 == oldTime) {
        unk18 = newTime;
    } else {
        unk18 = -1.0f;
        unk2C = 0.0f;
        unk28 = 0.0f;
        unk24 = 0.0f;
        unk20 = 0.0f;
    }
}

// fn_802B1B3C: body from https://github.com/jj-link/brawl src/havok/fn_802B1B3C.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
#include <havok/hkTransform.h>
namespace jj_fn_802B1B3C {
#pragma fp_contract on

struct Vec4 {
    float x, y, z, w;
};

struct Matrix {
    float m[4][4];
} __attribute__((aligned(16)));

struct Object {
    Vec4* shape;
    void* unk4;
    Matrix* transform;
};

extern "C" void fn_803198C8(void*, Vec4*, Vec4*, Matrix*, Vec4*);
extern "C" const float lbl_805A3F38;

// TODO: cleanup (shape from jj-link/brawl fn_802B1B3C)
extern "C" void fn_802B1B3C(Object* a, Object* b, void* c, Vec4* out) {
    Matrix matrix;
    Vec4 v;
    Vec4* sa = a->shape;
    Vec4* sb = b->shape;

    ((hkTransform*)(&matrix))->setMulInverseMul(*(const hkTransform*)(a->transform), *(const hkTransform*)(b->transform));
    fn_803198C8(c, sa, sb, &matrix, &v);

    Matrix* m = a->transform;
    float x = v.x, y = v.y, z = v.z;

    out->x = z * m->m[2][0] + (x * m->m[0][0] + y * m->m[1][0]);
    out->y = z * m->m[2][1] + (x * m->m[0][1] + y * m->m[1][1]);
    out->z = z * m->m[2][2] + (x * m->m[0][2] + y * m->m[1][2]);
    out->w = lbl_805A3F38;

    float ra = sa->w;
    float rb = sb->w;
    out->w = (v.w - ra) - rb;
}
} // namespace jj_fn_802B1B3C
