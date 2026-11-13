// Havok translation unit hkBoxBoxAgent.o (main.dol 0x802A0C68-0x802A1D88).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802A0C68   104  registerAgent   [map: hkBoxBoxAgent__registerAgent]
//   0x802A0CD0   212  createBoxBoxAgent   [map: hkBoxBoxAgent__createBoxBoxAgent]
//   0x802A0DA4    32  __dl   [map: hkIterativeLinearCastAgent____dl]
//   0x802A0E7C   144  cleanup   [map: hkBoxBoxAgent__cleanup]
//   0x802A0F0C    92  __dt   [map: hkBoxBoxAgent____dt]
//   0x802A0F68  1100  processCollision   [map: hkBoxBoxAgent__processCollision]
//   0x802A13B4  1168  staticGetClosestPoints   [map: hkBoxBoxAgent__staticGetClosestPoints]
//   0x802A1844    20  getClosestPoints   [map: hkBoxBoxAgent__getClosestPoints]
//   0x802A1858  1292  staticGetPenetrations   [map: hkBoxBoxAgent__staticGetPenetrations]
//   0x802A1D64    20  getPenetrations   [map: hkBoxBoxAgent__getPenetrations]
//   0x802A1D78     4  updateShapeCollectionFilter   [map: hkCollisionAgent__updateShapeCollectionFilter]
//   0x802A1D7C    12  __sinit_\hkBoxBoxAgent_cpp   [map: hkBoxBoxAgentcpp____sinit_]

#include <havok/hkBoxBoxAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkGskfAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkBoxShape.h>
#include <havok/hkBoxBoxCollisionDetection.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCdPoint.h>
#include <havok/hkCdBodyPairCollector.h>

// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
// Contact manager view with the feature-id removal slot used by cleanup (0x18). HYPOTHESIS: the other slots are unnamed.
struct hkBoxBoxContactMgr : hkReferencedObject {
    virtual void unk10();
    virtual void unk14();
    virtual void removeFeature(u16 featureId); // 0x18
};

// HYPOTHESIS: the size scale that the box test multiplies the shape size with.
static const float kBoxSizeScale = 1.999f;

// Dynamically initialised (hkBool has a constructor): the unit's __sinit stores the false.
hkBool hkBoxBoxAgent::m_attemptToFindAllEdges = false;

void hkBoxBoxAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.create = (hkAgentFunc)createBoxBoxAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)hkIterativeLinearCastAgent::staticLinearCast;
    funcs.symmetricA = 0;
    funcs.symmetricB = 0;
    dispatcher->registerCollisionAgent(&funcs, HK_SHAPE_BOX, HK_SHAPE_BOX);
}

hkCollisionAgent* hkBoxBoxAgent::createBoxBoxAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                   hkCollisionInput& input, hkContactMgr* contactMgr) {
    const hkBoxShape* boxA = (const hkBoxShape*)bodyA.m_shape;
    const hkBoxShape* boxB = (const hkBoxShape*)bodyB.m_shape;
    if (input.unk8 >= kBoxSizeScale * boxA->m_halfExtents.w || input.unk8 >= kBoxSizeScale * boxB->m_halfExtents.w) {
        return hkGskfAgent::createGskfAgent(bodyA, bodyB, input, contactMgr);
    }
    return new hkBoxBoxAgent(contactMgr);
}

void hkBoxBoxAgent::cleanup() {
    for (int i = 0; i < m_manifold.m_numPoints; i++) {
        ((hkBoxBoxContactMgr*)unk8)->removeFeature(m_manifold.m_points[i].unk2);
    }
    delete this;
}

hkBoxBoxAgent::~hkBoxBoxAgent() {}

void hkBoxBoxAgent::processCollision(const hkCdBody& bodyA, const hkCdBody& bodyB, hkProcessCollisionInput& input,
                                     void* result) {
    HK_TIMER_BEGIN("BoxBox");
    const hkBoxShape* boxA = (const hkBoxShape*)bodyA.getShape();
    const hkBoxShape* boxB = (const hkBoxShape*)bodyB.getShape();
    hkVector4 rA4;
    rA4.setAll(boxA->m_radius);
    hkVector4 rA;
    rA.setAdd4(boxA->m_halfExtents, rA4);
    hkVector4 rB4;
    rB4.setAll(boxB->m_radius);
    hkVector4 rB;
    rB.setAdd4(boxB->m_halfExtents, rB4);
    hkTransform aTb;
    aTb.setMulInverseMul(bodyA.getTransform(), bodyB.getTransform());
    hkBoxBoxCollisionDetection detector(bodyA, bodyB, &input, (hkContactMgr*)unk8, (hkProcessCollisionOutput*)result,
                                        aTb, bodyA.getTransform(), rA, bodyB.getTransform(), rB, input.getTolerance());
    detector.calcManifold(m_manifold);
    HK_TIMER_END();
}

void hkBoxBoxAgent::staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                           void* collector) {
    HK_TIMER_BEGIN("BoxBox");
    const hkBoxShape* boxA = (const hkBoxShape*)bodyA.getShape();
    const hkBoxShape* boxB = (const hkBoxShape*)bodyB.getShape();
    hkVector4 rA4;
    rA4.setAll(boxA->m_radius);
    hkVector4 rA;
    rA.setAdd4(boxA->m_halfExtents, rA4);
    hkVector4 rB4;
    rB4.setAll(boxB->m_radius);
    hkVector4 rB;
    rB.setAdd4(boxB->m_halfExtents, rB4);
    hkTransform aTb;
    aTb.setMulInverseMul(bodyA.getTransform(), bodyB.getTransform());
    hkBoxBoxCollisionDetection detector(bodyA, bodyB, 0, 0, 0, aTb, bodyA.getTransform(), rA, bodyB.getTransform(), rB,
                                        input.getTolerance());
    hkCdPoint event;
    event.m_cdBodyA = &bodyA;
    event.m_cdBodyB = &bodyB;
    if (detector.calculateClosestPoint(event.m_contact)) {
        ((hkCdPointCollector*)collector)->addCdPoint(&event);
    }
    HK_TIMER_END();
}

void hkBoxBoxAgent::staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                          void* collector) {
    HK_TIMER_BEGIN("BoxBox");
    const hkBoxShape* boxA = (const hkBoxShape*)bodyA.getShape();
    const hkBoxShape* boxB = (const hkBoxShape*)bodyB.getShape();
    hkVector4 rA4;
    rA4.setAll(boxA->m_radius);
    hkVector4 rA;
    rA.setAdd4(boxA->m_halfExtents, rA4);
    hkVector4 rB4;
    rB4.setAll(boxB->m_radius);
    hkVector4 rB;
    rB.setAdd4(boxB->m_halfExtents, rB4);
    hkTransform aTb;
    aTb.setMulInverseMul(bodyA.getTransform(), bodyB.getTransform());
    hkBoxBoxCollisionDetection detector(bodyA, bodyB, 0, 0, 0, aTb, bodyA.getTransform(), rA, bodyB.getTransform(), rB,
                                        input.getTolerance());
    if (detector.getPenetrations()) {
        ((hkCdBodyPairCollector*)collector)->addCdBodyPair(bodyA, bodyB);
    }
    HK_TIMER_END();
}

void hkBoxBoxAgent::getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    staticGetClosestPoints(unk0, unk1, unk2, unk3);
}

void hkBoxBoxAgent::getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}
