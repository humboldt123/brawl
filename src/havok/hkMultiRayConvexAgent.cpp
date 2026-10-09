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
#include <havok/hkShapeType.h>

// Stand-in for hkCollisionDispatcher::registerCollisionAgent (not recovered yet; takes the dispatcher as this).
extern "C" void fn_802CC0EC(hkCollisionDispatcher* dispatcher, void* funcs, int typeA, int typeB);
// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0();

namespace {
typedef void (*AgentFunc)();

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
struct AgentFuncs {
    AgentFunc create;                 // 0x00
    AgentFunc staticGetPenetrations;  // 0x04
    AgentFunc staticGetClosestPoints; // 0x08
    AgentFunc staticLinearCast;       // 0x0C
    u8 symmetricA;                    // 0x10
    u8 symmetricB;                    // 0x11
};
} // namespace

void hkMultiRayConvexAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    AgentFuncs symmetric;
    symmetric.create = (AgentFunc)createConvexMultiRayAgent;
    symmetric.staticGetPenetrations = (AgentFunc)hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetPenetrations;
    symmetric.staticGetClosestPoints = (AgentFunc)hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetClosestPoints;
    symmetric.staticLinearCast = (AgentFunc)hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticLinearCast;
    symmetric.symmetricA = 1;
    symmetric.symmetricB = 1;
    fn_802CC0EC(dispatcher, &symmetric, 1, HK_SHAPE_MULTI_RAY);

    AgentFuncs plain;
    plain.create = (AgentFunc)createMultiRayConvexAgent;
    plain.staticGetPenetrations = (AgentFunc)staticGetPenetrations;
    plain.staticGetClosestPoints = (AgentFunc)staticGetClosestPoints;
    plain.staticLinearCast = (AgentFunc)fn_802B7FD0;
    plain.symmetricA = 0;
    plain.symmetricB = 1;
    fn_802CC0EC(dispatcher, &plain, HK_SHAPE_MULTI_RAY, 1);
}

// The allocation size is stored before the constructor runs; the convex x multi-ray agent is the
// symmetric variant (its vtable is stored after the constructor).
hkMultiRayConvexAgent* hkMultiRayConvexAgent::createConvexMultiRayAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr) {
    hkMultiRayConvexAgent* agent = (hkMultiRayConvexAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkMultiRayConvexAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkMultiRayConvexAgent);
    ::new (agent) hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_(a1, a0, a2, contactMgr);
    return agent;
}

hkMultiRayConvexAgent* hkMultiRayConvexAgent::createMultiRayConvexAgent(void* a0, void* a1, void* a2, hkContactMgr* contactMgr) {
    hkMultiRayConvexAgent* agent = (hkMultiRayConvexAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkMultiRayConvexAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkMultiRayConvexAgent);
    ::new (agent) hkMultiRayConvexAgent(a0, a1, a2, contactMgr);
    return agent;
}

void hkMultiRayConvexAgent::cleanup() {
    int count = m_count;
    for (int i = 0; i < count; i++) {
        u32 slot = m_slots[i];
        if (slot != 0xFFFF) {
            ((hkContactMgr*)unk8)->unk18();
        }
    }
    delete this;
}

void hkMultiRayConvexAgent::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}
