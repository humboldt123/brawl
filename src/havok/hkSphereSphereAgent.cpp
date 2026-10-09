// Havok translation unit hkSphereSphereAgent.o (main.dol 0x802C6490-0x802C6F98).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C6490   104  registerAgent   [map: hkSphereSphereAgent__registerAgent]
//   0x802C64F8   120  createSphereSphereAgent   [map: hkSphereSphereAgent__createSphereSphereAgent]
//   0x802C6570   116  cleanup   [map: hkSphereSphereAgent__cleanup]
//   0x802C65E4    92  __dt   [map: hkSphereSphereAgent____dt]
//   0x802C6640   752  processCollision   [map: hkSphereSphereAgent__processCollision]
//   0x802C6930   632  getClosestPoints   [map: hkSphereSphereAgent__getClosestPoints]
//   0x802C6BA8   632  staticGetClosestPoints   [map: hkSphereSphereAgent__staticGetClosestPoints]
//   0x802C6E20    20  getPenetrations   [map: hkSphereSphereAgent__getPenetrations]
//   0x802C6E34   356  staticGetPenetrations   [map: hkSphereSphereAgent__staticGetPenetrations]

#include <havok/hkSphereSphereAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>

// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0();

void hkSphereSphereAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.symmetricA = 0;
    funcs.symmetricB = 0;
    funcs.create = (hkAgentFunc)createSphereSphereAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
    dispatcher->registerCollisionAgent(&funcs, HK_SHAPE_SPHERE, HK_SHAPE_SPHERE);
}

hkSphereSphereAgent* hkSphereSphereAgent::createSphereSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSphereSphereAgent(contactMgr);
}

// MATCH-ONLY: the release slot (0x18 of the contact manager) takes the 16-bit slot value as its argument.
// Same local view as hkCapsuleCapsuleAgent.cpp; hkContactMgr in hkCollisionAgent.h declares unk18() without one.
struct hkContactMgrKeyedSphere : hkReferencedObject {
    virtual void unk10();
    virtual void unk14();
    virtual void unk18(u16 key);
};

void hkSphereSphereAgent::cleanup() {
    u16 slot = unkC;
    if (slot != 0xFFFF) {
        ((hkContactMgrKeyedSphere*)unk8)->unk18(slot);
        unkC = 0xFFFF;
    }
    delete this;
}

hkSphereSphereAgent::~hkSphereSphereAgent() {}

void hkSphereSphereAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}
