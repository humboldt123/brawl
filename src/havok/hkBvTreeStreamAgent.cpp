// Havok translation unit hkBvTreeStreamAgent.o (main.dol 0x802A99B0-0x802AA9B8).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802A99B0   164  __ct   [map: hkBvTreeStreamAgent____ct]
//   0x802A9A54     8  getInstance   [map: hkMemory__getInstance]
//   0x802A9AF0   204  registerAgent   [map: hkBvTreeStreamAgent__registerAgent]
//   0x802A9BBC   204  registerConvexListAgent   [map: hkBvTreeStreamAgent__registerConvexListAgent]
//   0x802A9C88   204  registerMultiRayAgent   [map: hkBvTreeStreamAgent__registerMultiRayAgent]
//   0x802A9D54   132  createBvTreeShapeAgent   [map: hkBvTreeStreamAgent__createBvTreeShapeAgent]
//   0x802A9DD8   156  __dt   [map: hkBvTreeStreamAgent____dt]
//   0x802A9E74   124  createShapeBvAgent   [map: hkBvTreeStreamAgent__createShapeBvAgent]
//   0x802A9EF0    20  getPenetrations   [map: hkBvTreeStreamAgent__getPenetrations]
//   0x802A9F04    20  getClosestPoints   [map: hkBvTreeStreamAgent__getClosestPoints]
//   0x802A9F18    24  linearCast   [map: hkBvTreeStreamAgent__linearCast]
//   0x802A9F30    88  cleanup   [map: hkBvTreeStreamAgent__cleanup]
//   0x802A9F88   748  processCollision   [map: hkBvTreeStreamAgent__processCollision]
//   0x802AA274    12  getInstance   [map: hkMonitorStream__getInstance]
//   0x802AA280    28  memoryAvailable   [map: hkMonitorStream__memoryAvailable]
//   0x802AA29C     8  getEnd   [map: hkMonitorStream__getEnd]
//   0x802AA2A4    12  setTime   [map: hkMonitorStream12TimerCommandFv__setTime]
//   0x802AA2B0     8  setEnd   [map: hkMonitorStream__setEnd]
//   0x802AA2B8     8  getMotionState   [map: hkCdBody__getMotionState]
//   0x802AA2C0     4  getTransform   [map: hkMotionState__getTransform]
//   0x802AA2C4     4  __ct   [map: hkTransform____ct1]
//   0x802AA2C8     8  getShape   [map: hkCdBody__getShape]
//   0x802AA2D0    44  getAvailableMemory   [map: hkMemory__getAvailableMemory]
//   0x802AA2FC     8  getShapeCollection   [map: hkBvTreeShape__getShapeCollection]
//   0x802AA304     4  __ct   [map: hkAgent3ProcessInput____ct]
//   0x802AA308    28  __ct   [map: hkInplaceArray_Ui_128_____ct]
//   0x802AA324    32  pushBackUnchecked   [map: hkArray_Ui___pushBackUnchecked]
//   0x802AA344     8  getSize   [map: hkArray_Ui___getSize]
//   0x802AA34C     8  getSize   [map: hkArray_Q214hkAgent1nTrack14SectorDirEntry___getSize]
//   0x802AA354     8  begin   [map: hkArray_Ui___begin]
//   0x802AA35C   108  updateShapeCollectionFilter   [map: hkBvTreeStreamAgent__updateShapeCollectionFilter]
//   0x802AA3C8     8  invalidateTim   [map: hkBvTreeStreamAgent__invalidateTim]
//   0x802AA3D0     8  warpTime   [map: hkBvTreeStreamAgent__warpTime]
//   0x802AA3D8   168  calcStatistics   [map: hkBvTreeStreamAgent__calcStatistics]
//   0x802AA480   352  linearCast   [map: hkSymmetricAgent_19hkBvTreeStreamAgent___linearCast]
//   0x802AA5E0    72  getPenetrations   [map: hkSymmetricAgentLinearCast_19hkBvTreeStreamAgent___getPenetrations]
//   0x802AA628    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_19hkBvTreeStreamAgent___getClosestPoints]
//   0x802AA670   412  processCollision   [map: hkSymmetricAgentLinearCast_19hkBvTreeStreamAgent___processCollision]
//   0x802AA80C    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_19hkBvTreeStreamAgent___updateShapeCollectionFilter]
//   0x802AA81C   164  __dt   [map: hkSymmetricAgent_19hkBvTreeStreamAgent_____dt]
//   0x802AA8C0   248  registerAgent   [map: hkMoppBvTreeStreamAgent__registerAgent]

#include <havok/hkBvTreeStreamAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkBvTreeAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkBvAgent.h>

// HYPOTHESIS: hkAgent1nMachine helpers (map names hkAgent1nMachine_Create / _Destroy) that work on m_list.
extern "C" void fn_802FCB14(void* list);
extern "C" void fn_802FC6AC(void* list, int a, int b);
extern "C" void fn_802FC824(void* list);
extern "C" void fn_802FC988(void* list);
// Dispatcher registration (map name hkCollisionDispatcher registration, not recovered yet).

// HYPOTHESIS: registration record passed to the collision dispatcher (same layout as hkPhantomAgent.cpp).

hkBvTreeStreamAgent::~hkBvTreeStreamAgent() {}

void hkBvTreeStreamAgent::cleanup() {
    fn_802FC6AC(&m_list, unkC, unk8);
    delete this;
}

extern "C" void fn_802FEA54(void* subObject, void* args);

struct FilterArgs {
    void* a;
    void* b;
    u32 bValue;
    void* c;
    int unk8;
};

void hkBvTreeStreamAgent::updateShapeCollectionFilter(void* a, void* b, void* c) {
    void* inner = *(void**)b;
    unk10[3] = 0.0f;
    unk10[2] = 0.0f;
    unk10[1] = 0.0f;
    unk10[0] = 0.0f;
    unk10[7] = 0.0f;
    unk10[6] = 0.0f;
    unk10[5] = 0.0f;
    unk10[4] = 0.0f;
    u32 bValue = *(u32*)((char*)inner + 0x10);
    FilterArgs args;
    args.a = a;
    args.b = b;
    args.c = c;
    args.unk8 = unk8;
    args.bValue = bValue;
    fn_802FEA54(&m_list, &args);
}

// Both forward the list to hkAgent1nMachine (map names hkAgent1nMachine_InvalidateTim / _WarpTime).
void hkBvTreeStreamAgent::invalidateTim(void* arg) {
    fn_802FC824(&m_list);
}

void hkBvTreeStreamAgent::warpTime(float t0, float t1, void* arg) {
    fn_802FC988(&m_list);
}

// BYTES: the symmetric wrappers call these forwarders; without the pragma they are inlined there.
#pragma push
#pragma auto_inline off
void hkBvTreeStreamAgent::getPenetrations(void* a, void* b, void* c, void* d) {
    hkBvTreeAgent::staticGetPenetrations(a, b, c, d);
}

void hkBvTreeStreamAgent::getClosestPoints(void* a, void* b, void* c, void* d) {
    hkBvTreeAgent::staticGetClosestPoints(a, b, c, d);
}
#pragma pop

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkBvTreeStreamAgentVariant::linearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkBvTreeStreamAgent::linearCast(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        hkBvTreeStreamAgent::linearCast(b, a, &flippedInput, &flipped, 0);
    }
}

// BYTES: the symmetric linear cast calls this forwarder; without the pragma it is inlined there.
#pragma push
#pragma auto_inline off
void hkBvTreeStreamAgent::linearCast(void* a, void* b, void* c, void* d, void* e) {
    hkBvTreeAgent::staticLinearCast(a, b, c, d, e);
}
#pragma pop

#pragma dont_inline on
hkBvTreeStreamAgent::hkBvTreeStreamAgent(void* unused0, void* unused1, const int* src, int unk8Value)
    : hkCollisionAgent(unk8Value), m_list(&m_inlineEntry, 0, 1) {
    unkC = *src;
    unk10[3] = 0.0f;
    unk10[2] = 0.0f;
    unk10[1] = 0.0f;
    unk10[0] = 0.0f;
    unk10[7] = 0.0f;
    unk10[6] = 0.0f;
    unk10[5] = 0.0f;
    unk10[4] = 0.0f;
    fn_802FCB14(&m_list);
}
#pragma dont_inline reset

hkBvTreeStreamAgent* hkBvTreeStreamAgent::createBvTreeShapeAgent(void* a, void* b, const int* c, int d) {
    hkBvTreeStreamAgent* agent = (hkBvTreeStreamAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkBvTreeStreamAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkBvTreeStreamAgent);
    return new (agent) hkBvTreeStreamAgentVariant(b, a, c, d);
}

hkBvTreeStreamAgent* hkBvTreeStreamAgent::createShapeBvAgent(void* a, void* b, const int* c, int d) {
    hkBvTreeStreamAgent* agent = (hkBvTreeStreamAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkBvTreeStreamAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkBvTreeStreamAgent);
    new (agent) hkBvTreeStreamAgent(a, b, c, d);
    return agent;
}


// Registers the agent for BV tree x convex pairs in both orders.
void hkBvTreeStreamAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs reg;
    reg.create = (hkAgentFunc)&hkBvTreeStreamAgent::createBvTreeShapeAgent;
    reg.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations;
    reg.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints;
    reg.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticLinearCast;
    reg.symmetricA = true;
    reg.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&reg, HK_SHAPE_BV_TREE, HK_SHAPE_CONVEX);
    reg.create = (hkAgentFunc)&hkBvTreeStreamAgent::createShapeBvAgent;
    reg.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    reg.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    reg.staticLinearCast = (hkAgentFunc)hkBvTreeAgent::staticLinearCast;
    reg.symmetricA = false;
    reg.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&reg, HK_SHAPE_CONVEX, HK_SHAPE_BV_TREE);
}

// Registers the agent for BV tree x convex list pairs in both orders.
void hkBvTreeStreamAgent::registerConvexListAgent(void* dispatcher) {
    hkAgentFuncs reg;
    hkAgentFuncs reverseReg;
    reg.create = (hkAgentFunc)&hkBvTreeStreamAgent::createBvTreeShapeAgent;
    reg.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations;
    reg.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints;
    reg.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticLinearCast;
    reg.symmetricA = true;
    reg.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&reg, HK_SHAPE_BV_TREE, HK_SHAPE_CONVEX_LIST);
    reverseReg.create = (hkAgentFunc)&hkBvTreeStreamAgent::createShapeBvAgent;
    reverseReg.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    reverseReg.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    reverseReg.staticLinearCast = (hkAgentFunc)hkBvTreeAgent::staticLinearCast;
    reverseReg.symmetricA = false;
    reverseReg.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&reverseReg, HK_SHAPE_CONVEX_LIST, HK_SHAPE_BV_TREE);
}

// Registers the agent for BV tree x multi ray pairs in both orders.
void hkBvTreeStreamAgent::registerMultiRayAgent(void* dispatcher) {
    hkAgentFuncs regAB;
    regAB.create = (hkAgentFunc)&hkBvTreeStreamAgent::createBvTreeShapeAgent;
    regAB.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations;
    regAB.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints;
    regAB.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticLinearCast;
    regAB.symmetricA = true;
    regAB.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&regAB, HK_SHAPE_BV_TREE, HK_SHAPE_MULTI_RAY);
    hkAgentFuncs regBA;
    regBA.create = (hkAgentFunc)&hkBvTreeStreamAgent::createShapeBvAgent;
    regBA.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    regBA.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    regBA.staticLinearCast = (hkAgentFunc)hkBvTreeAgent::staticLinearCast;
    regBA.symmetricA = false;
    regBA.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&regBA, HK_SHAPE_MULTI_RAY, HK_SHAPE_BV_TREE);
}

// Symmetric wrapper of hkBvTreeStreamAgent (map name hkSymmetricAgentLinearCast_19hkBvTreeStreamAgent___
// updateShapeCollectionFilter): swap the bodies and forward to the plain agent.
template <class T>
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(void* a, void* b, void* c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::updateShapeCollectionFilter(void*, void*, void*);
template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::processCollision(void*, void*, void*, void*);

template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::getPenetrations(void*, void*, void*, void*);
template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::getClosestPoints(void*, void*, void*, void*);

