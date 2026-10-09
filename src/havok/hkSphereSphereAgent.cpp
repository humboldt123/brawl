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

// Stand-in for hkCollisionDispatcher::registerCollisionAgent (not recovered yet; takes the dispatcher as this).
extern "C" void fn_802CC0EC(hkCollisionDispatcher* dispatcher, void* funcs, int typeA, int typeB);
// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0();

namespace {
typedef void (*AgentFunc)();

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
struct AgentFuncs {
    AgentFunc create;                // 0x00
    AgentFunc staticGetPenetrations; // 0x04
    AgentFunc staticGetClosestPoints; // 0x08
    AgentFunc staticLinearCast;      // 0x0C
    u8 unk10;                        // 0x10
    u8 unk11;                        // 0x11
};
} // namespace

void hkSphereSphereAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    AgentFuncs funcs;
    funcs.create = (AgentFunc)createSphereSphereAgent;
    funcs.staticGetPenetrations = (AgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (AgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (AgentFunc)fn_802B7FD0;
    funcs.unk10 = 0;
    funcs.unk11 = 0;
    fn_802CC0EC(dispatcher, &funcs, HK_SHAPE_SPHERE, HK_SHAPE_SPHERE);
}

hkSphereSphereAgent* hkSphereSphereAgent::createSphereSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSphereSphereAgent(contactMgr);
}

void hkSphereSphereAgent::cleanup() {
    u16 slot = unkC;
    if (slot != 0xFFFF) {
        ((hkContactMgr*)unk8)->unk18();
        unkC = 0xFFFF;
    }
    delete this;
}

hkSphereSphereAgent::~hkSphereSphereAgent() {}

void hkSphereSphereAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}
