// Havok translation unit hkListAgent.o (main.dol 0x802B85EC-0x802B8E78).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B85EC   204  registerAgent   [map: hkListAgent__registerAgent]
//   0x802B86B8   132  createListAAgent   [map: hkListAgent__createListAAgent]
//   0x802B873C   124  createListBAgent   [map: hkListAgent__createListBAgent]
//   0x802B87B8   132  __ct   [map: hkListAgent____ct]
//   0x802B883C    88  cleanup   [map: hkListAgent__cleanup]
//   0x802B8894     8  invalidateTim   [map: hkListAgent__invalidateTim]
//   0x802B889C     8  warpTime   [map: hkListAgent__warpTime]
//   0x802B88A4   832  processCollision   [map: hkListAgent__processCollision]
//   0x802B8BE4    20  getClosestPoints   [map: hkListAgent__getClosestPoints]
//   0x802B8BF8   176  staticGetClosestPoints   [map: hkListAgent__staticGetClosestPoints]
//   0x802B8CA8    24  linearCast   [map: hkListAgent__linearCast]
//   0x802B8CC0   176  staticLinearCast   [map: hkListAgent__staticLinearCast]
//   0x802B8D70    20  getPenetrations   [map: hkListAgent__getPenetrations]
//   0x802B8D84   176  staticGetPenetrations   [map: hkListAgent__staticGetPenetrations]
//   0x802B8E34    68  updateShapeCollectionFilter   [map: hkListAgent__updateShapeCollectionFilter]

#include <havok/hkListAgent.h>
#include <havok/hkListShape.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkMotionState.h>
#include <havok/hkSweptTransformUtil.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkShapeCollectionAgent.h>
#include <havok/hkUnionFind.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>

// The list in A goes to the shape collection agent's static queries; the reverse record uses their symmetric forms.
void hkListAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs listAny;
    listAny.create = (hkAgentFunc)createListAAgent;
    listAny.staticGetPenetrations = (hkAgentFunc)hkShapeCollectionAgent::staticGetPenetrations;
    listAny.staticGetClosestPoints = (hkAgentFunc)hkShapeCollectionAgent::staticGetClosestPoints;
    listAny.staticLinearCast = (hkAgentFunc)hkShapeCollectionAgent::staticLinearCast;
    listAny.symmetricA = true;
    listAny.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&listAny, HK_SHAPE_LIST, HK_SHAPE_ALL);
    hkAgentFuncs anyList;
    anyList.create = (hkAgentFunc)createListBAgent;
    anyList.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetPenetrations;
    anyList.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetClosestPoints;
    anyList.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticLinearCast;
    anyList.symmetricA = false;
    anyList.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&anyList, HK_SHAPE_ALL, HK_SHAPE_LIST);
}

hkListAgent* hkListAgent::createListAAgent(void* a, void* b, void* c, int d) {
    hkListAgent* agent = (hkListAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkListAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkListAgent);
    return new (agent) hkSymmetricAgent_11hkListAgent_(b, a, c, d);
}

hkListAgent* hkListAgent::createListBAgent(void* a, void* b, void* c, int d) {
    hkListAgent* agent = (hkListAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkListAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkListAgent);
    new (agent) hkListAgent(a, b, c, d);
    return agent;
}

// HYPOTHESIS: hkAgent1nMachine_Create (map name): sets up an empty track.
extern "C" void fn_802FCB14(hkAgent1nTrack* track);

// HYPOTHESIS: the track is constructed a second time in place before hkAgent1nMachine_Create (the target initialises
// the in-place array twice, the second time behind a null test of the member address).
hkListAgent::hkListAgent(void* a, void* b, void* c, int unk8Value) : hkCollisionAgent(unk8Value) {
    unkC = (int)((hkCollisionInput*)c)->m_dispatcher;
    new (&m_agentTrack) hkAgent1nTrack;
    fn_802FCB14(&m_agentTrack);
}

// Stand-ins for the member functions of the embedded object at 0x10 (other unit, not recovered yet).
extern "C" void fn_802FC824(void* self, void* arg);
extern "C" void fn_802FC988(void* self, float t0, float t1, void* arg);
extern "C" void fn_802FC6AC(void* subObject, int pairData, int unk8);
extern "C" void fn_802FEA54(void* subObject, void* args);

// Argument block handed to fn_802FEA54 by updateShapeCollectionFilter (lives on the stack there).
struct FilterArgs {
    const hkCdBody* a;         // 0x00
    const hkCdBody* b;         // 0x04
    u32 bValue;                // 0x08 first word of b
    hkCollisionInput* d; // 0x0C
    int unk8;     // 0x10
};

void hkListAgent::cleanup() {
    fn_802FC6AC(&m_agentTrack, unkC, unk8);
    delete this;
}

void hkListAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& d) {
    FilterArgs args;
    args.a = &a;
    args.b = &b;
    args.d = &d;
    args.unk8 = unk8;
    args.bValue = (u32)b.m_shape;
    fn_802FEA54(&m_agentTrack, &args);
}

void hkListAgent::invalidateTim(void* arg) {
    fn_802FC824(&m_agentTrack, arg);
}

void hkListAgent::warpTime(float t0, float t1, void* arg) {
    fn_802FC988(&m_agentTrack, t0, t1, arg);
}

// Runs the track on every child of the list in B: the hit list is all child keys, ended by the invalid key.
void hkListAgent::processCollision(const hkCdBody& bodyA, const hkCdBody& bodyB, hkProcessCollisionInput& input,
                                   void* output) {
    HK_TIMER_BEGIN("list");
    const hkListShape* list = (const hkListShape*)bodyB.getShape();
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
    int size = list->m_childInfo.getSize();
    hkLocalBuffer<u32> hitList(size + 1);
    for (int i = 0; i < size; i++) {
        hitList[i] = i;
    }
    hitList[size] = 0xFFFFFFFF;
    fn_802FCCF0(&m_agentTrack, &in3, list, hitList.m_data, size, output);
    HK_TIMER_END();
}

void hkListAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    staticGetClosestPoints(a, b, c, (hkPenetrationTarget*)target);
}

void hkListAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    staticGetPenetrations(a, b, c, (hkPenetrationTarget*)target);
}

void hkListAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    staticLinearCast(a, b, c, (hkPenetrationTarget*)target, d);
}

// The static queries treat the list as a shape collection (with the bodies the other way round).
void hkListAgent::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c,
                                         hkPenetrationTarget* target) {
    HK_TIMER_BEGIN("ListAgent");
    hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetClosestPoints(a, b, c, target);
    HK_TIMER_END();
}

void hkListAgent::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c,
                                   hkPenetrationTarget* target, void* d) {
    HK_TIMER_BEGIN("ListAgent");
    hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticLinearCast(a, b, c, target, d);
    HK_TIMER_END();
}

void hkListAgent::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c,
                                        hkPenetrationTarget* target) {
    HK_TIMER_BEGIN("ListAgent");
    hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::staticGetPenetrations(a, b, c, target);
    HK_TIMER_END();
}
