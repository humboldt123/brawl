// Havok translation unit hkBvAgent.o (main.dol 0x802A1ED4-0x802A3A4C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802A1ED4   256  __ct   [map: hkBvAgent____ct]
//   0x802A1FD4    32  __dl   [map: hkCollisionAgent____dl]
//   0x802A1FF4   204  registerAgent   [map: hkBvAgent__registerAgent]
//   0x802A20C0   124  createBvShapeAgent   [map: hkBvAgent__createBvShapeAgent]
//   0x802A213C   132  createShapeBvAgent   [map: hkBvAgent__createShapeBvAgent]
//   0x802A21C0    92  __dt   [map: hkBvAgent____dt]
//   0x802A221C   128  cleanup   [map: hkBvAgent__cleanup]
//   0x802A229C   104  invalidateTim   [map: hkBvAgent__invalidateTim]
//   0x802A2304   136  warpTime   [map: hkBvAgent__warpTime]
//   0x802A238C    32  removePoint   [map: hkBvAgent__removePoint]
//   0x802A23AC    32  commitPotential   [map: hkBvAgent__commitPotential]
//   0x802A23CC    32  createZombie   [map: hkBvAgent__createZombie]
//   0x802A23EC   616  processCollision   [map: hkBvAgent__processCollision]
//   0x802A26B0   664  linearCast   [map: hkBvAgent__linearCast]
//   0x802A29A4   572  staticLinearCast   [map: hkBvAgent__staticLinearCast]
//   0x802A2BE0   616  getClosestPoints   [map: hkBvAgent__getClosestPoints]
//   0x802A2E48   524  staticGetClosestPoints   [map: hkBvAgent__staticGetClosestPoints]
//   0x802A3054   228  getPenetrations   [map: hkBvAgent__getPenetrations]
//   0x802A3138   344  staticGetPenetrations   [map: hkBvAgent__staticGetPenetrations]
//   0x802A3290   192  updateShapeCollectionFilter   [map: hkBvAgent__updateShapeCollectionFilter]
//   0x802A3350   352  linearCast   [map: hkSymmetricAgent_9hkBvAgent___linearCast]
//   0x802A34B0    72  getPenetrations   [map: hkSymmetricAgentLinearCast_9hkBvAgent___getPenetrations]
//   0x802A34F8    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_9hkBvAgent___staticGetPenetrations]
//   0x802A3540    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_9hkBvAgent___getClosestPoints]
//   0x802A3588    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_9hkBvAgent___staticGetClosestPoints]
//   0x802A35D0   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_9hkBvAgent___staticLinearCast]
//   0x802A3730   412  processCollision   [map: hkSymmetricAgentLinearCast_9hkBvAgent___processCollision]
//   0x802A38CC    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_9hkBvAgent___updateShapeCollectionFilter]
//   0x802A39F0    92  __dt   [map: hkSymmetricAgent_9hkBvAgent_____dt]

#include <havok/hkBvAgent.h>

void hkBvAgent::cleanup() {
    m_childA->cleanup();
    if (m_childB != 0) {
        m_childB->cleanup();
        m_childB = 0;
    }
    delete this;
}

void hkBvAgent::invalidateTim(void* arg) {
    m_childA->invalidateTim(arg);
    if (m_childB != 0) {
        m_childB->invalidateTim(arg);
    }
}

void hkBvAgent::warpTime(float t0, float t1, void* arg) {
    m_childA->warpTime(t0, t1, arg);
    if (m_childB != 0) {
        m_childB->warpTime(t0, t1, arg);
    }
}

void hkBvAgent::removePoint(void* arg) {
    if (m_childB != 0) {
        m_childB->removePoint(arg);
    }
}

void hkBvAgent::commitPotential(void* arg) {
    if (m_childB != 0) {
        m_childB->commitPotential(arg);
    }
}

void hkBvAgent::createZombie(void* arg) {
    if (m_childB != 0) {
        m_childB->createZombie(arg);
    }
}

hkBvAgent* hkBvAgent::createBvShapeAgent(void* a1, void* a2, void* a3, hkContactMgr* contactMgr) {
    hkBvAgent* agent = (hkBvAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkBvAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkBvAgent);
    ::new (agent) hkBvAgent(a1, a2, a3, contactMgr);
    return agent;
}

hkBvAgent* hkBvAgent::createShapeBvAgent(void* a1, void* a2, void* a3, hkContactMgr* contactMgr) {
    hkBvAgent* agent = new hkSymmetricBvAgent(a2, a1, a3, contactMgr);
    return agent;
}

// Sub-agents and the penetration target are passed around as in the original. The wrappers
// reorder the pair (a, b) and forward the target inside a hkSymmetricTarget.
template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    hkSymmetricTarget wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    hkSymmetricTarget wrapped(target);
    T::staticGetPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target) {
    hkSymmetricTarget wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target) {
    hkSymmetricTarget wrapped(target);
    T::staticGetClosestPoints(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(void* a, void* b, void* c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template struct hkSymmetricAgentLinearCast<hkBvAgent>;

namespace {
typedef void (*AgentFunc)();

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
struct AgentFuncs {
    AgentFunc create;                 // 0x00
    AgentFunc staticGetPenetrations;  // 0x04
    AgentFunc staticGetClosestPoints; // 0x08
    AgentFunc staticLinearCast;       // 0x0C
    bool symmetricA;                  // 0x10 HYPOTHESIS
    bool symmetricB;                  // 0x11 HYPOTHESIS
};
} // namespace

// Stand-in for the dispatcher registration function (not recovered yet; takes the dispatcher as first argument).
extern "C" void fn_802CC0EC(void* dispatcher, AgentFuncs* funcs, int typeA, int typeB);
// Stand-ins for the linear cast statics of the symmetric and plain BV agents (not written yet).
extern "C" void fn_802A35D0();
extern "C" void fn_802A29A4();

// Registers the BV agent for the pair of shape types in both orders: the symmetric variant for
// (any, 0x16) and the plain variant for (0x16, any).
void hkBvAgent::registerAgent(void* dispatcher) {
    AgentFuncs symmetric;
    symmetric.create = (AgentFunc)createShapeBvAgent;
    symmetric.staticGetPenetrations = (AgentFunc)hkSymmetricAgentLinearCast<hkBvAgent>::staticGetPenetrations;
    symmetric.staticGetClosestPoints = (AgentFunc)hkSymmetricAgentLinearCast<hkBvAgent>::staticGetClosestPoints;
    symmetric.staticLinearCast = (AgentFunc)fn_802A35D0;
    symmetric.symmetricA = true;
    symmetric.symmetricB = true;
    fn_802CC0EC(dispatcher, &symmetric, -1, 0x16);

    AgentFuncs plain;
    plain.create = (AgentFunc)createBvShapeAgent;
    plain.staticGetPenetrations = (AgentFunc)staticGetPenetrations;
    plain.staticGetClosestPoints = (AgentFunc)staticGetClosestPoints;
    plain.staticLinearCast = (AgentFunc)fn_802A29A4;
    plain.symmetricA = false;
    plain.symmetricB = true;
    fn_802CC0EC(dispatcher, &plain, 0x16, -1);
}
