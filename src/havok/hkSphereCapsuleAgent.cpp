// Havok translation unit hkSphereCapsuleAgent.o (main.dol 0x802C3E88-0x802C6490).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C3E88    92  __dt   [map: hkSymmetricAgentLinearCast_16hkSphereBoxAgent_____dt]
//   0x802C3EE4   204  registerAgent   [map: hkSphereCapsuleAgent__registerAgent]
//   0x802C3FB0   120  createCapsuleSphereAgent   [map: hkSphereCapsuleAgent__createCapsuleSphereAgent]
//   0x802C4028    92  __dt   [map: hkSphereCapsuleAgent____dt]
//   0x802C4084   120  createSphereCapsuleAgent   [map: hkSphereCapsuleAgent__createSphereCapsuleAgent]
//   0x802C40FC   104  cleanup   [map: hkSphereCapsuleAgent__cleanup]
//   0x802C4164  1640  getClosestPoints   [map: hkSphereCapsuleAgent__getClosestPoints]
//   0x802C47CC  1640  staticGetClosestPoints   [map: hkSphereCapsuleAgent__staticGetClosestPoints]
//   0x802C4E34  1100  staticGetPenetrations   [map: hkSphereCapsuleAgent__staticGetPenetrations]
//   0x802C5280    20  getPenetrations   [map: hkSphereCapsuleAgent__getPenetrations]
//   0x802C5294  1768  processCollision   [map: hkSphereCapsuleAgent__processCollision]
//   0x802C597C    72  getPenetrations   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent___getPenetrations]
//   0x802C59C4    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent___staticGetPenetrations]
//   0x802C5A0C    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent___getClosestPoints]
//   0x802C5A54    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent___staticGetClosestPoints]
//   0x802C5A9C   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent___staticLinearCast]
//   0x802C5BFC  2100  processCollision   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent___processCollision]
//   0x802C6430     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent___updateShapeCollectionFilter]
//   0x802C6434    92  __dt   [map: hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_____dt]

#include <havok/hkSphereBoxAgent.h>
#include <havok/hkSphereCapsuleAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>

// Stand-ins for the symmetric linear cast (staticLinearCast, not written yet) and the linear cast of other units.
extern "C" void fn_802C5A9C();
extern "C" void fn_802B7FD0();

void hkSphereCapsuleAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs capsule;
    capsule.create = (hkAgentFunc)createCapsuleSphereAgent;
    capsule.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::staticGetPenetrations;
    capsule.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::staticGetClosestPoints;
    capsule.staticLinearCast = (hkAgentFunc)fn_802C5A9C;
    capsule.symmetricA = 1;
    capsule.symmetricB = 0;
    dispatcher->registerCollisionAgent(&capsule, HK_SHAPE_CAPSULE, HK_SHAPE_SPHERE);

    hkAgentFuncs sphere;
    sphere.create = (hkAgentFunc)createSphereCapsuleAgent;
    sphere.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    sphere.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    sphere.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    sphere.symmetricA = 0;
    sphere.symmetricB = 0;
    dispatcher->registerCollisionAgent(&sphere, HK_SHAPE_SPHERE, HK_SHAPE_CAPSULE);
}

hkSphereCapsuleAgent* hkSphereCapsuleAgent::createCapsuleSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSphereCapsuleAgent(contactMgr);
}

hkSphereCapsuleAgent* hkSphereCapsuleAgent::createSphereCapsuleAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_(contactMgr);
}

// MATCH-ONLY: the release slot (0x18 of the contact manager) takes the 16-bit slot value as its argument.
// Same local view as hkCapsuleCapsuleAgent.cpp; hkContactMgr in hkCollisionAgent.h declares unk18() without one.
struct hkContactMgrKeyedSphere : hkReferencedObject {
    virtual void unk10();
    virtual void unk14();
    virtual void unk18(u16 key);
};

void hkSphereCapsuleAgent::cleanup() {
    u16 slot = unkC;
    if (slot != 0xFFFF) {
        ((hkContactMgrKeyedSphere*)unk8)->unk18(slot);
    }
    delete this;
}

void hkSphereCapsuleAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereCapsuleAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereCapsuleAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkSphereCapsuleAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkSphereCapsuleAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::~hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_() {}

// Destructor of the symmetric box agent (map places it in this unit).
hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::~hkSymmetricAgentLinearCast_16hkSphereBoxAgent_() {}
