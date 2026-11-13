// Havok translation unit hkSphereTriangleAgent.o (main.dol 0x802C6F98-0x802C8E9C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C6F98   204  registerAgent   [map: hkSphereTriangleAgent__registerAgent]
//   0x802C7064   184  createTriangleSphereAgent   [map: hkSphereTriangleAgent__createTriangleSphereAgent]
//   0x802C711C    92  __dt   [map: hkSphereTriangleAgent____dt]
//   0x802C7178   168  createSphereTriangleAgent   [map: hkSphereTriangleAgent__createSphereTriangleAgent]
//   0x802C7220   104  cleanup   [map: hkSphereTriangleAgent__cleanup]
//   0x802C7288  1116  getClosestPoints   [map: hkSphereTriangleAgent__getClosestPoints]
//   0x802C76E4  1136  staticGetClosestPoints   [map: hkSphereTriangleAgent__staticGetClosestPoints]
//   0x802C7B54   432  getPenetrations   [map: hkSphereTriangleAgent__getPenetrations]
//   0x802C7D04   440  staticGetPenetrations   [map: hkSphereTriangleAgent__staticGetPenetrations]
//   0x802C7EBC  1240  processCollision   [map: hkSphereTriangleAgent__processCollision]
//   0x802C8394    72  getPenetrations   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___getPenetrations]
//   0x802C83DC    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___staticGetPenetrations]
//   0x802C8424    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___getClosestPoints]
//   0x802C846C    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___staticGetClosestPoints]
//   0x802C84B4   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___staticLinearCast]
//   0x802C8614  1572  processCollision   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___processCollision]
//   0x802C8C38     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent___updateShapeCollectionFilter]
//   0x802C8C3C    92  __dt   [map: hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_____dt]
//   0x802C8C98   216  addCdPoint   [map: hkSymmetricAgentFlipCollector__addCdPoint]
//   0x802C8D70   216  addCdPoint   [map: hkSymmetricAgentFlipCastCollector__addCdPoint]
//   0x802C8E48    84  addCdBodyPair   [map: hkSymmetricAgentFlipBodyCollector__addCdBodyPair]

#include <havok/hkSphereTriangleAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCdBodyPairCollector.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkSphereShape.h>
#include <havok/hkTriangleShape.h>
#include <havok/hkTransform.h>
#include <havok/hkCollideTriangleUtil.h>

// Result of the point-triangle query (hkCollideTriangleUtil::closestPointTriangle). HYPOTHESIS: the vector is the
// separating direction; only the distance at 0x10 is read here.
struct hkClosestPointTriangleResult {
    hkVector4 m_direction; // 0x00
    hkReal m_distance;     // 0x10
};
// Stand-in for hkCollideTriangleUtil::closestPointTriangle (other unit, not written yet): closest point of the
// triangle to the point, using the cache built by hkCollideTriangleUtil::setupClosestPointTriangleCache.
extern "C" int fn_8032561C(const hkVector4* point, const hkVector4* triangle, const void* cache,
                           hkClosestPointTriangleResult* result);
// Stand-ins for the symmetric linear cast and the linear cast of other units (staticLinearCast not written yet).
void hkSphereTriangleAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs triangle;
    triangle.create = (hkAgentFunc)createTriangleSphereAgent;
    triangle.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetPenetrations;
    triangle.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetClosestPoints;
    triangle.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticLinearCast;
    triangle.symmetricA = 1;
    triangle.symmetricB = 0;
    dispatcher->registerCollisionAgent(&triangle, HK_SHAPE_TRIANGLE, HK_SHAPE_SPHERE);

    hkAgentFuncs sphere;
    sphere.create = (hkAgentFunc)createSphereTriangleAgent;
    sphere.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    sphere.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    sphere.staticLinearCast = (hkAgentFunc)hkIterativeLinearCastAgent::staticLinearCast;
    sphere.symmetricA = 0;
    sphere.symmetricB = 0;
    dispatcher->registerCollisionAgent(&sphere, HK_SHAPE_SPHERE, HK_SHAPE_TRIANGLE);
}

// The create functions inline this; the dead `mr r30, r3` there is the object pointer kept for the exception cleanup
// of the base (the unit is built with -Cpp_exceptions on).
hkSphereTriangleAgent::hkSphereTriangleAgent(const hkCdBody& triangleBody, hkContactMgr* contactMgr)
    : hkIterativeLinearCastAgent((int)contactMgr) {
    unkC = 0xFFFF;
    const hkTriangleShape* triangle = (const hkTriangleShape*)triangleBody.m_shape;
    hkCollideTriangleUtil::setupClosestPointTriangleCache(&triangle->m_vertexA, (hkVector4*)unk10);
}

hkSphereTriangleAgent* hkSphereTriangleAgent::createTriangleSphereAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                                        hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_(bodyA, contactMgr);
}

hkSphereTriangleAgent* hkSphereTriangleAgent::createSphereTriangleAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                                        hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSphereTriangleAgent(bodyB, contactMgr);
}

// Reports the pair when the closest point of the triangle to the sphere centre (in the triangle's frame) is closer
// than the sum of the radii. The agent keeps the triangle cache at 0x10; the static form builds it on the stack.
void hkSphereTriangleAgent::getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    HK_TIMER_BEGIN("SphereTri");
    const hkSphereShape* sphere = (const hkSphereShape*)(&unk0)->m_shape;
    const hkTriangleShape* triangle = (const hkTriangleShape*)(&unk1)->m_shape;

    hkVector4 centre;
    centre._setTransformedInversePos(*(const hkTransform*)(&unk1)->m_motion,
                                     ((const hkTransform*)(&unk0)->m_motion)->m_translation);
    hkClosestPointTriangleResult result;
    fn_8032561C(&centre, &triangle->m_vertexA, unk10, &result);
    hkReal triangleRadius = triangle->m_radius;
    hkReal sphereRadius = sphere->m_radius;
    hkReal radius = sphereRadius + triangleRadius;
    if (result.m_distance < radius) {
        ((hkCdBodyPairCollector*)unk3)->addCdBodyPair(unk0, unk1);
    }
    HK_TIMER_END();
}

void hkSphereTriangleAgent::staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    HK_TIMER_BEGIN("SphereTri");
    const hkSphereShape* sphere = (const hkSphereShape*)(&unk0)->m_shape;
    const hkTriangleShape* triangle = (const hkTriangleShape*)(&unk1)->m_shape;
    u8 cache[16];
    hkCollideTriangleUtil::setupClosestPointTriangleCache(&triangle->m_vertexA, (hkVector4*)cache);

    hkVector4 centre;
    centre._setTransformedInversePos(*(const hkTransform*)(&unk1)->m_motion,
                                     ((const hkTransform*)(&unk0)->m_motion)->m_translation);
    hkClosestPointTriangleResult result;
    fn_8032561C(&centre, &triangle->m_vertexA, cache, &result);
    hkReal triangleRadius = triangle->m_radius;
    hkReal sphereRadius = sphere->m_radius;
    hkReal radius = sphereRadius + triangleRadius;
    if (result.m_distance < radius) {
        ((hkCdBodyPairCollector*)unk3)->addCdBodyPair(unk0, unk1);
    }
    HK_TIMER_END();
}

void hkSphereTriangleAgent::cleanup() {
    u16 slot = unkC;
    if (slot != 0xFFFF) {
        ((hkContactMgr*)unk8)->removeContactPoint(slot);
    }
    delete this;
}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkSphereTriangleAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkSphereTriangleAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereTriangleAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticGetClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereTriangleAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::~hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_() {}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkIterativeLinearCastAgent::staticLinearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkIterativeLinearCastAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_802C8E48.cpp.
// TODO: cleanup (shape from jj-link/brawl fn_802C8E48)
void hkSymmetricAgentFlipBodyCollector::addCdBodyPair(void* bodyA, void* bodyB) {
    hkCdBodyPairCollector* collector = (hkCdBodyPairCollector*)unk8;
    collector->addCdBodyPair(*(const hkCdBody*)bodyB, *(const hkCdBody*)bodyA);
    unk4 = ((hkCdBodyPairCollector*)unk8)->m_earlyOut.m_bool;
}
