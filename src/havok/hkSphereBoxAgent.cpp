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
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>

// Stand-in for the symmetric linear cast (staticLinearCast, not written yet).
extern "C" void fn_802C3B88();
// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0();

void hkSphereBoxAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs box;
    box.create = (hkAgentFunc)createBoxSphereAgent;
    box.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetPenetrations;
    box.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetClosestPoints;
    box.staticLinearCast = (hkAgentFunc)fn_802C3B88;
    box.symmetricA = 1;
    box.symmetricB = 0;
    dispatcher->registerCollisionAgent(&box, HK_SHAPE_BOX, HK_SHAPE_SPHERE);

    hkAgentFuncs sphere;
    sphere.create = (hkAgentFunc)createSphereBoxAgent;
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
    if (unkC != 0xFFFF) {
        ((hkContactMgr*)unk8)->unk18();
    }
    delete this;
}

void hkSphereBoxAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereBoxAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkSphereBoxAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkSphereBoxAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkSphereBoxAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkSphereBoxAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}
