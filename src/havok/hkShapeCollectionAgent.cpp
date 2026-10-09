// Havok translation unit hkShapeCollectionAgent.o (main.dol 0x802C0A38-0x802C1F7C).
// Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C0A38   204  registerAgent   [map: hkShapeCollectionAgent__registerAgent]
//   0x802C0B04   124  createListAAgent   [map: hkShapeCollectionAgent__createListAAgent]
//   0x802C0B80   132  createListBAgent   [map: hkShapeCollectionAgent__createListBAgent]
//   0x802C0C04   152  __dt   [map: hkShapeCollectionAgent____dt]
//   0x802C0D2C   552  __ct   [map: hkShapeCollectionAgent____ct]
//   0x802C0F54   148  cleanup   [map: hkShapeCollectionAgent__cleanup]
//   0x802C0FE8   132  invalidateTim   [map: hkShapeCollectionAgent__invalidateTim]
//   0x802C106C   164  warpTime   [map: hkShapeCollectionAgent__warpTime]
//   0x802C1110   352  processCollision   [map: hkShapeCollectionAgent__processCollision]
//   0x802C1270   352  getClosestPoints   [map: hkShapeCollectionAgent__getClosestPoints]
//   0x802C13D0   512  staticGetClosestPoints   [map: hkShapeCollectionAgent__staticGetClosestPoints]
//   0x802C15D0   360  linearCast   [map: hkShapeCollectionAgent__linearCast]
//   0x802C1738   520  staticLinearCast   [map: hkShapeCollectionAgent__staticLinearCast]
//   0x802C1940   376  getPenetrations   [map: hkShapeCollectionAgent__getPenetrations]
//   0x802C1AB8   536  staticGetPenetrations   [map: hkShapeCollectionAgent__staticGetPenetrations]
//   0x802C1CD0   684  updateShapeCollectionFilter   [map: hkShapeCollectionAgent__updateShapeCollectionFilter]

#include <havok/hkShapeCollectionAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>

// Stand-ins for functions in other TUs or not yet recovered (the names are the map names).
extern "C" void fn_802B0FE0(void* a, void* b, void* c, void* d);
extern "C" void fn_802B1028(void* a, void* b, void* c, void* d, void* e);
extern "C" void fn_802B1070(void* a, void* b, void* c, void* d);
extern "C" void fn_802C1AB8(void* a, void* b, void* c, void* d);
extern "C" void fn_802C13D0(void* a, void* b, void* c, void* d);
extern "C" void fn_802C1738(void* a, void* b, void* c, void* d, void* e);
// Dispatcher registration (not recovered yet).

// HYPOTHESIS: registration record passed to the collision dispatcher (same layout as hkPhantomAgent.cpp).

hkShapeCollectionAgent* hkShapeCollectionAgent::createListAAgent(void* a, void* b, void* c, int d) {
    hkShapeCollectionAgent* agent = (hkShapeCollectionAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkShapeCollectionAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkShapeCollectionAgent);
    new (agent) hkShapeCollectionAgent(a, b, c, d);
    return agent;
}

hkShapeCollectionAgent* hkShapeCollectionAgent::createListBAgent(void* a, void* b, void* c, int d) {
    hkShapeCollectionAgent* agent = (hkShapeCollectionAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkShapeCollectionAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkShapeCollectionAgent);
    if (agent != 0) {
        new (agent) hkShapeCollectionAgentVariant(b, a, c, d);
    }
    return agent;
}

// Registers the agent for shape collection x any shape (-1) and the reverse.
void hkShapeCollectionAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs regAB;
    regAB.create = (hkAgentFunc)&hkShapeCollectionAgent::createListBAgent;
    regAB.staticGetPenetrations = (hkAgentFunc)fn_802B0FE0;
    regAB.staticGetClosestPoints = (hkAgentFunc)fn_802B1028;
    regAB.staticLinearCast = (hkAgentFunc)fn_802B1070;
    regAB.symmetricA = true;
    regAB.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&regAB, HK_SHAPE_ALL, HK_SHAPE_COLLECTION);
    hkAgentFuncs regBA;
    regBA.create = (hkAgentFunc)&hkShapeCollectionAgent::createListAAgent;
    regBA.staticGetPenetrations = (hkAgentFunc)fn_802C1AB8;
    regBA.staticGetClosestPoints = (hkAgentFunc)fn_802C13D0;
    regBA.staticLinearCast = (hkAgentFunc)fn_802C1738;
    regBA.symmetricA = false;
    regBA.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&regBA, HK_SHAPE_COLLECTION, HK_SHAPE_ALL);
}
