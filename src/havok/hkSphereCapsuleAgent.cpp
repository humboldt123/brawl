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

// Stand-in for hkCollisionDispatcher::registerCollisionAgent (not recovered yet; takes the dispatcher as this).
extern "C" void fn_802CC0EC(hkCollisionDispatcher* dispatcher, void* funcs, int typeA, int typeB);
// Stand-ins for the symmetric linear cast (staticLinearCast, not written yet) and the linear cast of other units.
extern "C" void fn_802C5A9C();
extern "C" void fn_802B7FD0();

namespace {
typedef void (*AgentFunc)();

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
struct AgentFuncs {
    AgentFunc create;                 // 0x00
    AgentFunc staticGetPenetrations;  // 0x04
    AgentFunc staticGetClosestPoints; // 0x08
    AgentFunc staticLinearCast;       // 0x0C
    u8 unk10;                         // 0x10
    u8 unk11;                         // 0x11
};
} // namespace

void hkSphereCapsuleAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    AgentFuncs capsule;
    capsule.create = (AgentFunc)createCapsuleSphereAgent;
    capsule.staticGetPenetrations = (AgentFunc)hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::staticGetPenetrations;
    capsule.staticGetClosestPoints = (AgentFunc)hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_::staticGetClosestPoints;
    capsule.staticLinearCast = (AgentFunc)fn_802C5A9C;
    capsule.unk10 = 1;
    capsule.unk11 = 0;
    fn_802CC0EC(dispatcher, &capsule, HK_SHAPE_CAPSULE, HK_SHAPE_SPHERE);

    AgentFuncs sphere;
    sphere.create = (AgentFunc)createSphereCapsuleAgent;
    sphere.staticGetPenetrations = (AgentFunc)staticGetPenetrations;
    sphere.staticGetClosestPoints = (AgentFunc)staticGetClosestPoints;
    sphere.staticLinearCast = (AgentFunc)fn_802B7FD0;
    sphere.unk10 = 0;
    sphere.unk11 = 0;
    fn_802CC0EC(dispatcher, &sphere, HK_SHAPE_SPHERE, HK_SHAPE_CAPSULE);
}

hkSphereCapsuleAgent* hkSphereCapsuleAgent::createCapsuleSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSphereCapsuleAgent(contactMgr);
}

hkSphereCapsuleAgent* hkSphereCapsuleAgent::createSphereCapsuleAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_20hkSphereCapsuleAgent_(contactMgr);
}

void hkSphereCapsuleAgent::cleanup() {
    if (unkC != 0xFFFF) {
        ((hkContactMgr*)unk8)->unk18();
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
