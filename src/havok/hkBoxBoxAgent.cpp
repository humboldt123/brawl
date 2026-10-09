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

// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0();

// Shape view for the box size test: the value at 0x1C of the shape (HYPOTHESIS: half extent w component).
// Collision input view: tolerance at 0x08 (HYPOTHESIS).
struct hkBoxBoxInput {
    u8 unk0[8];
    float tolerance; // 0x08
};

// Contact manager view with the feature-id removal slot used by cleanup (0x18). HYPOTHESIS: the other slots are unnamed.
struct hkBoxBoxContactMgr : hkReferencedObject {
    virtual void unk10();
    virtual void unk14();
    virtual void removeFeature(u16 featureId); // 0x18
};

// HYPOTHESIS: the size scale that the box test multiplies the shape size with.
static const float kBoxSizeScale = 2.0f;

void hkBoxBoxAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.create = (hkAgentFunc)createBoxBoxAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    funcs.symmetricA = 0;
    funcs.symmetricB = 0;
    dispatcher->registerCollisionAgent(&funcs, HK_SHAPE_BOX, HK_SHAPE_BOX);
}

hkCollisionAgent* hkBoxBoxAgent::createBoxBoxAgent(hkCdBody* bodyA, hkCdBody* bodyB, const void* input, hkContactMgr* contactMgr) {
    const hkBoxBoxInput* in = (const hkBoxBoxInput*)input;
    if (in->tolerance >= kBoxSizeScale * *(float*)((u8*)bodyA->m_shape + 0x1C) ||
        in->tolerance >= kBoxSizeScale * *(float*)((u8*)bodyB->m_shape + 0x1C)) {
        return hkGskfAgent::createGskfAgent(bodyA, bodyB, (void*)input, contactMgr);
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

void hkBoxBoxAgent::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetClosestPoints(unk0, unk1, unk2, unk3);
}

void hkBoxBoxAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}
