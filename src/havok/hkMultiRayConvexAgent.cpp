// Havok translation unit hkMultiRayConvexAgent.o (main.dol 0x802B8E78-0x802BA204).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B8E78   204  registerAgent   [map: hkMultiRayConvexAgent__registerAgent]
//   0x802B8F44   404  __ct   [map: hkMultiRayConvexAgent____ct]
//   0x802B9168   132  createConvexMultiRayAgent   [map: hkMultiRayConvexAgent__createConvexMultiRayAgent]
//   0x802B91EC   152  __dt   [map: hkMultiRayConvexAgent____dt]
//   0x802B9284   124  createMultiRayConvexAgent   [map: hkMultiRayConvexAgent__createMultiRayConvexAgent]
//   0x802B9300   164  cleanup   [map: hkMultiRayConvexAgent__cleanup]
//   0x802B93A4  1012  getClosestPoints   [map: hkMultiRayConvexAgent__getClosestPoints]
//   0x802B9798  1012  staticGetClosestPoints   [map: hkMultiRayConvexAgent__staticGetClosestPoints]
//   0x802B9B8C    20  getPenetrations   [map: hkMultiRayConvexAgent__getPenetrations]
//   0x802B9BA0   664  staticGetPenetrations   [map: hkMultiRayConvexAgent__staticGetPenetrations]
//   0x802B9E38   972  processCollision   [map: hkMultiRayConvexAgent__processCollision]

#include <havok/hkMultiRayConvexAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkCdBody.h>
#include <havok/hkCdPoint.h>
#include <havok/hkCdBodyPairCollector.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkTransform.h>
#include <havok/hkMonitorStream.h>

// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0();

namespace {

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
} // namespace

// BYTES: the create functions call the constructor; without the pragma it is inlined into them.
#pragma push
#pragma auto_inline off
// One contact point id per ray of the multi-ray shape (body A), none held yet.
hkMultiRayConvexAgent::hkMultiRayConvexAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr)
    : hkCollisionAgent((int)contactMgr) {
    const hkMultiRayShape* shape = (const hkMultiRayShape*)((const hkCdBody*)a0)->m_shape;
    int numRays = shape->m_rays.getSize();
    m_contactPointIds.setSize(numRays);
    for (int i = 0; i < numRays; i++) {
        m_contactPointIds[i] = 0xFFFF;
    }
}
#pragma pop

hkMultiRayConvexAgent::~hkMultiRayConvexAgent() {}

void hkMultiRayConvexAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs symmetric;
    symmetric.create = (hkAgentFunc)createConvexMultiRayAgent;
    symmetric.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetPenetrations;
    symmetric.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetClosestPoints;
    symmetric.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticLinearCast;
    symmetric.symmetricA = 1;
    symmetric.symmetricB = 1;
    dispatcher->registerCollisionAgent(&symmetric, 1, HK_SHAPE_MULTI_RAY);

    hkAgentFuncs plain;
    plain.create = (hkAgentFunc)createMultiRayConvexAgent;
    plain.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    plain.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    plain.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    plain.symmetricA = 0;
    plain.symmetricB = 1;
    dispatcher->registerCollisionAgent(&plain, HK_SHAPE_MULTI_RAY, 1);
}

// The allocation size is stored before the constructor runs; the convex x multi-ray agent is the
// symmetric variant (its vtable is stored after the constructor).
hkMultiRayConvexAgent* hkMultiRayConvexAgent::createConvexMultiRayAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr) {
    hkMultiRayConvexAgent* agent = (hkMultiRayConvexAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkMultiRayConvexAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkMultiRayConvexAgent);
    return ::new (agent) hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_(a1, a0, a2, contactMgr);
}

hkMultiRayConvexAgent* hkMultiRayConvexAgent::createMultiRayConvexAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr) {
    hkMultiRayConvexAgent* agent = (hkMultiRayConvexAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkMultiRayConvexAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkMultiRayConvexAgent);
    ::new (agent) hkMultiRayConvexAgent(a0, a1, a2, contactMgr);
    return agent;
}

void hkMultiRayConvexAgent::cleanup() {
    int count = m_contactPointIds.getSize();
    for (int i = 0; i < count; i++) {
        u16 slot = m_contactPointIds[i];
        if (slot != 0xFFFF) {
            ((hkContactMgr*)unk8)->removeContactPoint(slot);
        }
    }
    delete this;
}

// Each ray is taken into the convex shape's frame (body B) and cast against it; a hit becomes a collision point
// in world space whose distance is the part of the ray beyond the hit, offset by the shape's penetration distance.
// The agent slot and the static function have the same body.

void hkMultiRayConvexAgent::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    const hkCdBody& bodyA = *(const hkCdBody*)unk0;
    const hkCdBody& bodyB = *(const hkCdBody*)unk1;
    HK_TIMER_BEGIN("multiRay-cvx");
    const hkMultiRayShape* shape = (const hkMultiRayShape*)bodyA.m_shape;
    const hkShape* convex = bodyB.m_shape;
    hkTransform aTb;
    aTb.setMulInverseMul(*(const hkTransform*)bodyB.m_motion, *(const hkTransform*)bodyA.m_motion);
    int numRays = shape->m_rays.getSize();
    const hkMultiRayShape::Ray* ray = shape->m_rays.begin();
    hkCdPointCollector& collector = *(hkCdPointCollector*)unk3;
    hkCdPoint point;
    point.m_cdBodyA = &bodyA;
    point.m_cdBodyB = &bodyB;
    hkShapeRayCastInput rayInput;
    rayInput.unk20 = 0;
    rayInput.unk24 = 0;
    for (int i = 0; i < numRays; i++) {
        hkShapeRayCastOutput output;
        rayInput.m_from._setTransformedPos(aTb, ray->m_start);
        rayInput.m_to._setTransformedPos(aTb, ray->m_end);
        // HYPOTHESIS: the slot takes the whole output; hkShape declares it by its leading normal.
        if (convex->castRay(rayInput, output.m_normal)) {
            const hkTransform& bTransform = *(const hkTransform*)bodyB.m_motion;
            hkVector4 hitPoint;
            hitPoint.setInterpolate4(rayInput.m_from, rayInput.m_to, output.m_hitFraction);
            point.m_contact.m_position._setTransformedPos(bTransform, hitPoint);
            point.m_contact.m_normal._setRotatedDir(bTransform.m_rotation, output.m_normal);
            point.m_contact.m_normal.w =
                (output.m_hitFraction - 1.0f) * ray->m_start.w + shape->m_rayPenetrationDistance;
            collector.addCdPoint(&point);
        }
        ray++;
    }
    HK_TIMER_END();
}

void hkMultiRayConvexAgent::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    const hkCdBody& bodyA = *(const hkCdBody*)unk0;
    const hkCdBody& bodyB = *(const hkCdBody*)unk1;
    HK_TIMER_BEGIN("multiRay-cvx");
    const hkMultiRayShape* shape = (const hkMultiRayShape*)bodyA.m_shape;
    const hkShape* convex = bodyB.m_shape;
    hkTransform aTb;
    aTb.setMulInverseMul(*(const hkTransform*)bodyB.m_motion, *(const hkTransform*)bodyA.m_motion);
    int numRays = shape->m_rays.getSize();
    const hkMultiRayShape::Ray* ray = shape->m_rays.begin();
    hkCdPointCollector& collector = *(hkCdPointCollector*)unk3;
    hkCdPoint point;
    point.m_cdBodyA = &bodyA;
    point.m_cdBodyB = &bodyB;
    hkShapeRayCastInput rayInput;
    rayInput.unk20 = 0;
    rayInput.unk24 = 0;
    for (int i = 0; i < numRays; i++) {
        hkShapeRayCastOutput output;
        rayInput.m_from._setTransformedPos(aTb, ray->m_start);
        rayInput.m_to._setTransformedPos(aTb, ray->m_end);
        // HYPOTHESIS: the slot takes the whole output; hkShape declares it by its leading normal.
        if (convex->castRay(rayInput, output.m_normal)) {
            const hkTransform& bTransform = *(const hkTransform*)bodyB.m_motion;
            hkVector4 hitPoint;
            hitPoint.setInterpolate4(rayInput.m_from, rayInput.m_to, output.m_hitFraction);
            point.m_contact.m_position._setTransformedPos(bTransform, hitPoint);
            point.m_contact.m_normal._setRotatedDir(bTransform.m_rotation, output.m_normal);
            point.m_contact.m_normal.w =
                (output.m_hitFraction - 1.0f) * ray->m_start.w + shape->m_rayPenetrationDistance;
            collector.addCdPoint(&point);
        }
        ray++;
    }
    HK_TIMER_END();
}

void hkMultiRayConvexAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}

// Reports the pair once for every ray that hits the convex shape, until the collector has seen enough.
void hkMultiRayConvexAgent::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkCdBodyPairCollector& collector = *(hkCdBodyPairCollector*)unk3;
    const hkCdBody& bodyA = *(const hkCdBody*)unk0;
    const hkCdBody& bodyB = *(const hkCdBody*)unk1;
    HK_TIMER_BEGIN("multiRay-cnvx-getPen");
    const hkMultiRayShape* shape = (const hkMultiRayShape*)bodyA.m_shape;
    const hkShape* convex = bodyB.m_shape;
    hkTransform aTb;
    aTb.setMulInverseMul(*(const hkTransform*)bodyB.m_motion, *(const hkTransform*)bodyA.m_motion);
    int numRays = shape->m_rays.getSize();
    const hkMultiRayShape::Ray* ray = shape->m_rays.begin();
    hkShapeRayCastInput rayInput;
    rayInput.unk20 = 0;
    rayInput.unk24 = 0;
    hkShapeRayCastOutput output;
    while (numRays-- && !collector.m_earlyOut) {
        rayInput.m_from._setTransformedPos(aTb, ray->m_start);
        rayInput.m_to._setTransformedPos(aTb, ray->m_end);
        if (convex->castRay(rayInput, output.m_normal)) {
            collector.addCdBodyPair(bodyA, bodyB);
        }
        ray++;
    }
    HK_TIMER_END();
}
