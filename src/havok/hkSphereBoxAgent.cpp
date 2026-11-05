// Havok translation unit hkSphereBoxAgent.o (main.dol 0x802C23B8-0x802C3E88).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C23B8   204  registerAgent   [map: hkSphereBoxAgent__registerAgent]
//   0x802C2484   120  createBoxSphereAgent   [map: hkSphereBoxAgent__createBoxSphereAgent]
//   0x802C24FC    92  __dt   [map: hkSphereBoxAgent____dt]
//   0x802C2558   120  createSphereBoxAgent   [map: hkSphereBoxAgent__createSphereBoxAgent]
//   0x802C25D0   104  cleanup   [map: hkSphereBoxAgent__cleanup]
//   0x802C2638  1536  processCollision   [map: hkSphereBoxAgent__processCollision]
//   0x802C2C38  1424  getClosestPoints   [map: hkSphereBoxAgent__getClosestPoints]
//   0x802C31C8  1424  staticGetClosestPoints   [map: hkSphereBoxAgent__staticGetClosestPoints]
//   0x802C3758   764  staticGetPenetrations   [map: hkSphereBoxAgent__staticGetPenetrations]
//   0x802C3A54    20  getPenetrations   [map: hkSphereBoxAgent__getPenetrations]
//   0x802C3A68    72  getPenetrations   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent___getPenetrations]
//   0x802C3AB0    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent___staticGetPenetrations]
//   0x802C3AF8    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent___getClosestPoints]
//   0x802C3B40    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent___staticGetClosestPoints]
//   0x802C3B88   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent___staticLinearCast]
//   0x802C3CE8   412  processCollision   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent___processCollision]
//   0x802C3E84     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent___updateShapeCollectionFilter]

#include <havok/hkSphereBoxAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCdBodyPairCollector.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkSphereShape.h>
#include <havok/hkBoxShape.h>
#include <havok/hkTransform.h>

// Stand-in for the symmetric linear cast (staticLinearCast, not written yet).
// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0(void* a, void* b, void* c, void* target, void* d);

void hkSphereBoxAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs box;
    box.create = (hkAgentFunc)createSphereBoxAgent;
    box.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetPenetrations;
    box.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetClosestPoints;
    box.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticLinearCast;
    box.symmetricA = 1;
    box.symmetricB = 0;
    dispatcher->registerCollisionAgent(&box, HK_SHAPE_BOX, HK_SHAPE_SPHERE);

    hkAgentFuncs sphere;
    sphere.create = (hkAgentFunc)createBoxSphereAgent;
    sphere.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    sphere.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    sphere.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    sphere.symmetricA = 0;
    sphere.symmetricB = 0;
    dispatcher->registerCollisionAgent(&sphere, HK_SHAPE_SPHERE, HK_SHAPE_BOX);
}

hkSphereBoxAgent* hkSphereBoxAgent::createBoxSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSphereBoxAgent(contactMgr);
}

hkSphereBoxAgent* hkSphereBoxAgent::createSphereBoxAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_16hkSphereBoxAgent_(contactMgr);
}

void hkSphereBoxAgent::cleanup() {
    u16 slot = unkC;
    if (slot != 0xFFFF) {
        ((hkContactMgr*)unk8)->removeContactPoint(slot);
    }
    delete this;
}

void hkSphereBoxAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkSphereBoxAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkSphereBoxAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereBoxAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereBoxAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

// Reports the pair when the sphere centre is closer to the box than the sum of the two convex radii.
// The centre is brought into box space, clamped to the half extents (keeping its signs) and moved back.
void hkSphereBoxAgent::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    HK_TIMER_BEGIN("SphereBox");

    const hkSphereShape* sphere = (const hkSphereShape*)((const hkCdBody*)unk0)->m_shape;
    const hkBoxShape* box = (const hkBoxShape*)((const hkCdBody*)unk1)->m_shape;
    const hkVector4& centre = ((const hkTransform*)((const hkCdBody*)unk0)->m_motion)->m_translation;

    hkVector4 local;
    local.setTransformedInversePos(*(const hkTransform*)((const hkCdBody*)unk1)->m_motion, centre);
    hkVector4 clamped;
    clamped.setAbs4(local);
    clamped.setMin4(clamped, box->m_halfExtents);
    clamped.setFlipSign4(clamped, local);

    hkVector4 nearest;
    nearest._setTransformedPos(*(const hkTransform*)((const hkCdBody*)unk1)->m_motion, clamped);
    hkVector4 offset;
    offset.setSub4(nearest, centre);
    hkReal distSq = offset.lengthSquared3();
    hkReal radius = sphere->m_radius + box->m_radius;
    if (distSq < radius * radius) {
        ((hkCdBodyPairCollector*)unk3)->addCdBodyPair(*(const hkCdBody*)unk0, *(const hkCdBody*)unk1);
    }
    HK_TIMER_END();
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticLinearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        fn_802B7FD0(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        fn_802B7FD0(b, a, &flippedInput, &flipped, 0);
    }
}

void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::processCollision(void* a, void* b, void* c, void* d) {
    hkSymmetricAgent_processCollision<hkSphereBoxAgent>(this, a, b, c, d);
}
