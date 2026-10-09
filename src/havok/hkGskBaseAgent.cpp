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
#include <havok/hkShape.h>
#include <havok/hkShapeType.h>
#include <havok/hkTransform.h>

// Stand-ins for the GSK setup helpers (other units, not recovered yet). fn_80287600 builds the relative transform
// of the two motions; fn_8031C8CC / fn_8031CD64 initialise the sub-object at 0x0C for a triangle / other shape pair.
extern "C" void fn_80287600(void* out, void* motionA, void* motionB);
extern "C" void fn_8031C8CC(void* self, void* shapeA, void* shapeB, void* relative);
extern "C" void fn_8031CD64(void* self, void* shapeA, void* shapeB, void* relative);

hkGskBaseAgent::hkGskBaseAgent(hkCdBody* bodyA, hkCdBody* bodyB, hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {
    hkTransform relative;
    hkShape* shapeA = bodyA->m_shape;
    hkShape* shapeB = bodyB->m_shape;
    fn_80287600(&relative, bodyA->m_motion, bodyB->m_motion);
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
    while (bodyA->m_parent != 0) {
        bodyA = bodyA->m_parent;
    }
    while (bodyB->m_parent != 0) {
        bodyB = bodyB->m_parent;
    }
    float valueA = *(float*)((u8*)bodyA + 0x20);
    float valueB = *(float*)((u8*)bodyB + 0x20);
    unk1C = valueA < valueB ? valueA : valueB;
}

void hkGskBaseAgent::processCollision(void* unk0, void* unk1, void* unk2) {}

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

hkGskBaseAgent::~hkGskBaseAgent() {}

void hkGskBaseAgent::warpTime(float oldTime, float newTime, void* arg) {
    if (unk18 == oldTime) {
        unk18 = newTime;
    } else {
        unk2C = 0.0f;
        unk18 = -1.0f;
        unk28 = 0.0f;
        unk24 = 0.0f;
        unk20 = 0.0f;
    }
}
