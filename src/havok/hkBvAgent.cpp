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
#include <havok/hkCollisionDispatcher.h>

void hkBvAgent::cleanup() {
    m_childA->cleanup();
    if (m_childB != 0) {
        m_childB->cleanup();
        m_childB = 0;
    }
    delete this;
}

// Argument block handed to the children: the first word comes from the object that a points to, the
// second and third words are copied from a, the last is a itself.
struct hkBvFilterArgs {
    void* first;   // 0x00
    void* second;  // 0x04
    u32 third;     // 0x08
    void* owner;   // 0x0C
};

#pragma dont_inline on
void hkBvAgent::updateShapeCollectionFilter(void* a, void* b, void* c) {
    hkBvFilterArgs args;
    args.third = *(u32*)((char*)a + 8);
    void* inner = *(void**)a;
    args.second = *(void**)((char*)a + 4);
    args.owner = a;
    args.first = *(void**)((char*)inner + 0xc);
    m_childA->updateShapeCollectionFilter(&args, b, c);
    if (m_childB != 0) {
        args.second = *(void**)((char*)args.owner + 4);
        args.first = *(void**)((char*)inner + 0x14);
        m_childB->updateShapeCollectionFilter(&args, b, c);
    }
}

#pragma dont_inline reset
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

void hkBvAgent::removePoint(u16 key) {
    if (m_childB != 0) {
        m_childB->removePoint(key);
    }
}

void hkBvAgent::commitPotential(u16 key) {
    if (m_childB != 0) {
        m_childB->commitPotential(key);
    }
}

void hkBvAgent::createZombie(u16 key) {
    if (m_childB != 0) {
        m_childB->createZombie(key);
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
void hkSymmetricAgentLinearCast<T>::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricTarget wrapped((hkPenetrationTarget*)target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    hkSymmetricTarget wrapped(target);
    T::staticGetPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricTarget wrapped((hkPenetrationTarget*)target);
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

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
} // namespace

// Stand-in for the dispatcher registration function (not recovered yet; takes the dispatcher as first argument).
// Stand-ins for the linear cast statics of the symmetric and plain BV agents (not written yet).
extern "C" void fn_802A35D0();
extern "C" void fn_802A29A4();

// Registers the BV agent for the pair of shape types in both orders: the symmetric variant for
// (any, 0x16) and the plain variant for (0x16, any).
void hkBvAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs symmetric;
    symmetric.create = (hkAgentFunc)createShapeBvAgent;
    symmetric.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast<hkBvAgent>::staticGetPenetrations;
    symmetric.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast<hkBvAgent>::staticGetClosestPoints;
    symmetric.staticLinearCast = (hkAgentFunc)fn_802A35D0;
    symmetric.symmetricA = true;
    symmetric.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&symmetric, -1, 0x16);

    hkAgentFuncs plain;
    plain.create = (hkAgentFunc)createBvShapeAgent;
    plain.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    plain.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    plain.staticLinearCast = (hkAgentFunc)fn_802A29A4;
    plain.symmetricA = false;
    plain.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&plain, 0x16, -1);
}
