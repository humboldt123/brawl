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
#include <havok/hkBvTreeShape.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkMotionState.h>
#include <havok/hkSweptTransformUtil.h>
#include <havok/hkAlgorithm.h>
#include <havok/hkMoppAgent.h>

// HYPOTHESIS: hkAgent1nMachine helpers (map names hkAgent1nMachine_Create / _Destroy) that work on m_list.
extern "C" void fn_802FCB14(void* list);
extern "C" void fn_802FC6AC(void* list, int a, int b);
extern "C" void fn_802FC824(void* list);
extern "C" void fn_802FC988(void* list);
// HYPOTHESIS: the invalid shape key (0xFFFFFFFF), appended as the end marker of the sorted hit list.
extern "C" const u32 lbl_8059EF58;
// Dispatcher registration (map name hkCollisionDispatcher registration, not recovered yet).

// HYPOTHESIS: registration record passed to the collision dispatcher (same layout as hkPhantomAgent.cpp).

hkBvTreeStreamAgent::~hkBvTreeStreamAgent() {}

void hkBvTreeStreamAgent::cleanup() {
    fn_802FC6AC(&m_list, unkC, unk8);
    delete this;
}

// BYTES: the whole function is compiled without inlining; every accessor (stream, body, array, memory) is a call to
// its out-of-line copy, which follow the function in the image.
#pragma push
#pragma dont_inline on
// Queries B's tree with A's aabb swept over the step and hands the sorted hit keys (ended by the invalid key) to the
// agent track, which keeps one child agent per hit. When the cached aabb still covers A the track only reruns its
// agents. Gives up for this step when the track might need more sectors (0x200 bytes each, one per four keys) than
// the memory has left, flagging the memory as out of memory.
void hkBvTreeStreamAgent::processCollision(const hkCdBody& bodyA, const hkCdBody& bodyB, hkProcessCollisionInput& input,
                                           void* output) {
    HK_TIMER_BEGIN_LIST_ACCESSORS("BvTree3", "QueryTree");
    hkAgent3ProcessInput in3;
    {
        in3.m_bodyA = &bodyA;
        in3.m_bodyB = &bodyB;
        in3.m_contactMgr = (hkContactMgr*)unk8;
        in3.m_input = &input;
        const hkMotionState* msA = bodyA.getMotionState();
        const hkMotionState* msB = bodyB.getMotionState();
        hkSweptTransformUtil::calcTimInfo(*msA, *msB, input.m_stepInfo.m_deltaTime, in3.m_linearTimInfo);
        in3.m_aTb.setMulInverseMul(msA->getTransform(), msB->getTransform());
    }
    hkInplaceArray<u32, 128> hitList;
    hitList.pushBackUnchecked(lbl_8059EF58);
    hkTransform bTa;
    bTa.setInverse(in3.m_aTb);
    int cached = hkBvTreeAgent::calcAabbAndQueryTree(bodyA, bodyB, bTa, in3.m_linearTimInfo, input, (hkAabb*)unk10,
                                                     hitList);
    HK_TIMER_SPLIT_LIST_ACCESSORS("Narrow");
    if (cached == 0) {
        int memNeeded = (hitList.getSize() / 4 + 1 - m_list.m_sectors.getSize()) * 0x200;
        if (memNeeded > hkMemory::getInstance().getAvailableMemory()) {
            hkMemory::getInstance().unk4 = 1;
            return;
        }
        hkAlgorithm::quickSort(hitList.begin(), hitList.getSize());
        const hkShapeCollection* collection = ((const hkBvTreeShape*)bodyB.getShape())->getShapeCollection();
        fn_802FCCF0(&m_list, &in3, collection, hitList.begin(), hitList.getSize(), output);
    } else {
        const hkShapeCollection* collection = ((const hkBvTreeShape*)bodyB.getShape())->getShapeCollection();
        fn_802FCCF0(&m_list, &in3, collection, 0, 0, output);
    }
    HK_TIMER_END_LIST_ACCESSORS();
}
#pragma pop

extern "C" void fn_802FEA54(void* subObject, void* args);

struct FilterArgs {
    const hkCdBody* a;
    const hkCdBody* b;
    u32 bValue;
    hkCollisionInput* c;
    int unk8;
};

void hkBvTreeStreamAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
    const void* inner = b.m_shape;
    unk10[3] = 0.0f;
    unk10[2] = 0.0f;
    unk10[1] = 0.0f;
    unk10[0] = 0.0f;
    unk10[7] = 0.0f;
    unk10[6] = 0.0f;
    unk10[5] = 0.0f;
    unk10[4] = 0.0f;
    u32 bValue = *(const u32*)((const char*)inner + 0x10);
    FilterArgs args;
    args.a = &a;
    args.b = &b;
    args.c = &c;
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

// Statistics collector slots used here (local view, as in hkBvTreeAgent.cpp). HYPOTHESIS: names.
struct hkStatisticsCollectorIface {
    virtual void unk08();
    virtual void addObject(const char* name, int type, const void* object);                      // 0x0C
    virtual void addArray(const char* name, int type, const void* data, int size, int capacity); // 0x10

    // An array's storage, counted only when the array owns it (the name is computed before the ownership test).
    template <typename T>
    void addArray(const char* name, int type, const hkArray<T>& array) {
        if ((array.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
            addArray(name, type, array.m_data, array.m_size * sizeof(T),
                     (array.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * sizeof(T));
        }
    }
};

// HYPOTHESIS: hkAgentMachineUtil::calc1nStatistics (map name): the statistics of the track's sectors and agents.
extern "C" void fn_802FEA60(const hkAgent1nTrack* track, hkStatisticsCollector* collector);

// The agent, its sector directory (when it owns its storage) and the track's agents.
void hkBvTreeStreamAgent::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("BvTreeAgt3", 8, this);
    ((hkStatisticsCollectorIface*)collector)->addArray("SectorPtrs", 8, m_list.m_sectors);
    fn_802FEA60(&m_list, collector);
}

// BYTES: the symmetric wrappers call these forwarders; without the pragma they are inlined there.
#pragma push
#pragma auto_inline off
void hkBvTreeStreamAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d) {
    hkBvTreeAgent::staticGetPenetrations(a, b, c, d);
}

void hkBvTreeStreamAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d) {
    hkBvTreeAgent::staticGetClosestPoints(a, b, c, d);
}
#pragma pop

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_19hkBvTreeStreamAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkBvTreeStreamAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkBvTreeStreamAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// BYTES: the symmetric linear cast calls this forwarder; without the pragma it is inlined there.
#pragma push
#pragma auto_inline off
void hkBvTreeStreamAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* d, void* e) {
    hkBvTreeAgent::staticLinearCast(a, b, c, d, e);
}
#pragma pop

// BYTES: the factories call this constructor, so it must not be auto-inlined into them; `dont_inline` would also stop
// the hkCollisionAgent constructor from being inlined here. m_list is an inplace array (its storage is addressed from
// the array's own this). NONMATCHING (~76%): the target keeps the hkCollisionAgent vtable store and loads *src after the
// list stores (the no-dead-store mode, see TIPS; trigger not found here).
#pragma push
#pragma auto_inline off
hkBvTreeStreamAgent::hkBvTreeStreamAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                         hkContactMgr* contactMgr)
    : hkCollisionAgent((int)contactMgr) {
    unkC = (int)input.m_dispatcher;
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
#pragma pop

hkBvTreeStreamAgent* hkBvTreeStreamAgent::createBvTreeShapeAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                                 hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSymmetricAgent_19hkBvTreeStreamAgent_(bodyB, bodyA, input, contactMgr);
}

hkBvTreeStreamAgent* hkBvTreeStreamAgent::createShapeBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                             hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkBvTreeStreamAgent(bodyA, bodyB, input, contactMgr);
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
    hkAgentFuncs rev;
    rev.create = (hkAgentFunc)&hkBvTreeStreamAgent::createShapeBvAgent;
    rev.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    rev.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    rev.staticLinearCast = (hkAgentFunc)hkBvTreeAgent::staticLinearCast;
    rev.symmetricA = false;
    rev.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&rev, HK_SHAPE_CONVEX, HK_SHAPE_BV_TREE);
}

// Registers the agent for BV tree x convex list pairs in both orders.
void hkBvTreeStreamAgent::registerConvexListAgent(void* dispatcher) {
    hkAgentFuncs reg;
    reg.create = (hkAgentFunc)&hkBvTreeStreamAgent::createBvTreeShapeAgent;
    reg.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations;
    reg.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints;
    reg.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticLinearCast;
    reg.symmetricA = true;
    reg.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&reg, HK_SHAPE_BV_TREE, HK_SHAPE_CONVEX_LIST);
    hkAgentFuncs reverseReg;
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
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::updateShapeCollectionFilter(const hkCdBody&, const hkCdBody&, hkCollisionInput&);
template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::processCollision(const hkCdBody&, const hkCdBody&, hkProcessCollisionInput&, void*);

template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::getPenetrations(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
template void hkSymmetricAgentLinearCast<hkBvTreeStreamAgent>::getClosestPoints(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);

// Registers the stream agent for MOPP x convex pairs in both orders (the MOPP agent's queries) and the MOPP agent for
// MOPP x MOPP pairs.
void hkMoppBvTreeStreamAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs moppConvex;
    moppConvex.create = (hkAgentFunc)&hkBvTreeStreamAgent::createBvTreeShapeAgent;
    moppConvex.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_11hkMoppAgent::staticGetPenetrations;
    moppConvex.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_11hkMoppAgent::staticGetClosestPoints;
    moppConvex.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_11hkMoppAgent::staticLinearCast;
    moppConvex.symmetricA = true;
    moppConvex.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&moppConvex, HK_SHAPE_MOPP, HK_SHAPE_CONVEX);
    hkAgentFuncs convexMopp;
    convexMopp.create = (hkAgentFunc)&hkBvTreeStreamAgent::createShapeBvAgent;
    convexMopp.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    convexMopp.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    convexMopp.staticLinearCast = (hkAgentFunc)hkMoppAgent::staticLinearCast;
    convexMopp.symmetricA = false;
    convexMopp.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&convexMopp, HK_SHAPE_CONVEX, HK_SHAPE_MOPP);
    hkAgentFuncs moppMopp;
    moppMopp.create = (hkAgentFunc)&hkMoppAgent::createBvBvAgent;
    moppMopp.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    moppMopp.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    moppMopp.staticLinearCast = (hkAgentFunc)hkMoppAgent::staticLinearCast;
    moppMopp.symmetricA = false;
    moppMopp.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&moppMopp, HK_SHAPE_MOPP, HK_SHAPE_MOPP);
}
