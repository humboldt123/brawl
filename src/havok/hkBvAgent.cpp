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
#include <havok/hkBvShape.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkFlagCdBodyPairCollector.h>
#include <havok/hkSimpleClosestContactCollector.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>

// The agent for the bounding volume (A) is made with the agent; the agent for the shape inside (B) only once the
// bounding volume is found overlapping. The bounding volume body replaces the shape of body A by the bounding
// volume shape.
// BYTES: the create functions call the constructor; without the pragma it is inlined into them.
#pragma push
#pragma auto_inline off
hkBvAgent::hkBvAgent(void* a1, void* a2, void* a3, hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {
    const hkCdBody* bodyA = (const hkCdBody*)a1;
    const hkCollisionInput* input = (const hkCollisionInput*)a3;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    m_childA = input->m_dispatcher->getNewCollisionAgent(bv, *(const hkCdBody*)a2, *input, contactMgr);
    m_childB = 0;
}
#pragma pop

// The shape inside is only processed while its bounding volume overlaps body B; its agent is made on the first
// overlap and dropped when the overlap ends.
void hkBvAgent::processCollision(void* a, void* b, void* c, void* d) {
    HK_TIMER_BEGIN_LIST("hkBvAgent", "checkBvShape");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkCollisionInput* input = (const hkCollisionInput*)c;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    hkFlagCdBodyPairCollector checker;
    m_childA->getPenetrations(&bv, b, c, &checker);
    if (checker.m_hit) {
        HK_TIMER_SPLIT_LIST("child");
        bv.m_shapeKey = bv.m_parent->m_shapeKey;
        bv.m_shape = bvShape->m_childContainer.m_childShape;
        if (m_childB == 0) {
            m_childB = input->m_dispatcher->getNewCollisionAgent(bv, *(const hkCdBody*)b, *input, (hkContactMgr*)unk8);
        }
        m_childB->processCollision(&bv, b, c, d);
    } else if (m_childB != 0) {
        m_childB->cleanup();
        m_childB = 0;
    }
    HK_TIMER_END_LIST();
}

void hkBvAgent::linearCast(void* a, void* b, void* c, void* target, void* d) {
    HK_TIMER_BEGIN_LIST("hkBvAgent", "checkBvShape");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkCollisionInput* input = (const hkCollisionInput*)c;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    hkSimpleClosestContactCollector checker;
    m_childA->linearCast(&bv, b, c, &checker, &checker);
    if (checker.m_hit) {
        HK_TIMER_SPLIT_LIST("child");
        bv.m_shapeKey = bv.m_parent->m_shapeKey;
        bv.m_shape = bvShape->m_childContainer.m_childShape;
        if (m_childB == 0) {
            m_childB = input->m_dispatcher->getNewCollisionAgent(bv, *(const hkCdBody*)b, *input, (hkContactMgr*)unk8);
        }
        m_childB->linearCast(&bv, b, c, target, d);
    } else if (m_childB != 0) {
        m_childB->cleanup();
        m_childB = 0;
    }
    HK_TIMER_END_LIST();
}

void hkBvAgent::staticLinearCast(void* a, void* b, void* c, void* target, void* d) {
    HK_TIMER_BEGIN_LIST("hkBvAgent", "checkBvShape");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkCdBody* bodyB = (const hkCdBody*)b;
    const hkCollisionInput* input = (const hkCollisionInput*)c;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    int typeA = bv.m_shape->getType();
    int typeB = bodyB->m_shape->getType();
    typedef void (*LinearCastFunc)(void*, void*, void*, void*, void*);
    LinearCastFunc bvCast = (LinearCastFunc)input->m_dispatcher->getLinearCastFunc(typeA, typeB);
    hkSimpleClosestContactCollector checker;
    bvCast(&bv, b, c, &checker, &checker);
    if (checker.m_hit) {
        HK_TIMER_SPLIT_LIST("child");
        bv.m_shapeKey = bv.m_parent->m_shapeKey;
        bv.m_shape = bvShape->m_childContainer.m_childShape;
        int childType = bv.m_shape->getType();
        ((LinearCastFunc)input->m_dispatcher->getLinearCastFunc(childType, typeB))(&bv, b, c, target, d);
    }
    HK_TIMER_END_LIST();
}

void hkBvAgent::getClosestPoints(void* a, void* b, void* c, void* target) {
    HK_TIMER_BEGIN_LIST("hkBvAgent", "checkBvShape");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkCollisionInput* input = (const hkCollisionInput*)c;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    hkFlagCdBodyPairCollector checker;
    m_childA->getPenetrations(&bv, b, c, &checker);
    if (checker.m_hit) {
        HK_TIMER_SPLIT_LIST("child");
        bv.m_shapeKey = bv.m_parent->m_shapeKey;
        bv.m_shape = bvShape->m_childContainer.m_childShape;
        if (m_childB == 0) {
            m_childB = input->m_dispatcher->getNewCollisionAgent(bv, *(const hkCdBody*)b, *input, (hkContactMgr*)unk8);
        }
        m_childB->getClosestPoints(&bv, b, c, target);
    } else if (m_childB != 0) {
        m_childB->cleanup();
        m_childB = 0;
    }
    HK_TIMER_END_LIST();
}

void hkBvAgent::staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target) {
    HK_TIMER_BEGIN_LIST("hkBvAgent", "checkBvShape");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkCdBody* bodyB = (const hkCdBody*)b;
    const hkCollisionInput* input = (const hkCollisionInput*)c;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    int typeA = bv.m_shape->getType();
    int typeB = bodyB->m_shape->getType();
    typedef void (*QueryFunc)(void*, void*, void*, void*);
    QueryFunc bvQuery = (QueryFunc)input->m_dispatcher->getGetPenetrationsFunc(typeA, typeB);
    hkFlagCdBodyPairCollector checker;
    bvQuery(&bv, b, c, &checker);
    if (checker.m_hit) {
        HK_TIMER_SPLIT_LIST("child");
        bv.m_shapeKey = bv.m_parent->m_shapeKey;
        bv.m_shape = bvShape->m_childContainer.m_childShape;
        int childType = bv.m_shape->getType();
        ((QueryFunc)input->m_dispatcher->getGetClosestPointsFunc(childType, typeB))(&bv, b, c, target);
    }
    HK_TIMER_END_LIST();
}

void hkBvAgent::cleanup() {
    m_childA->cleanup();
    if (m_childB != 0) {
        m_childB->cleanup();
        m_childB = 0;
    }
    delete this;
}

// Penetrations are those of the bounding volume: the query goes to the bounding volume agent with body A
// replaced by its bounding volume shape.
void hkBvAgent::getPenetrations(void* a, void* b, void* c, void* target) {
    HK_TIMER_BEGIN("hkBvAgent");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    m_childA->getPenetrations(&bv, b, c, target);
    HK_TIMER_END();
}

// Same, through the dispatcher's static penetration function for the (bounding volume, B) shape types. The
// first word of the collision input is the dispatcher.
void hkBvAgent::staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    HK_TIMER_BEGIN("hkBvAgent");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkBvShape* bvShape = (const hkBvShape*)bodyA->m_shape;
    hkCdBody bv(bodyA);
    bv.m_shapeKey = bodyA->m_shapeKey;
    bv.m_shape = bvShape->m_boundingVolumeShape;
    int typeA = bv.m_shape->getType();
    int typeB = ((const hkCdBody*)b)->m_shape->getType();
    hkCollisionDispatcher* dispatcher = *(hkCollisionDispatcher**)c;
    typedef void (*PenetrationsFunc)(void*, void*, void*, hkPenetrationTarget*);
    ((PenetrationsFunc)dispatcher->m_agentFuncs[((u8(*)[32])dispatcher->m_lookup190)[typeA][typeB]].staticGetPenetrations)(
        &bv, b, c, target);
    HK_TIMER_END();
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
// reorder the pair (a, b) and forward the target inside a flip collector (hkSymmetricAgentFlipCollectors.h).
template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::staticGetPenetrations(b, a, c, (hkPenetrationTarget*)&wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::staticGetClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::staticGetClosestPoints(b, a, c, (hkPenetrationTarget*)&wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(void* a, void* b, void* c) {
    T::updateShapeCollectionFilter(b, a, c);
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
template <class T>
void hkSymmetricAgentLinearCast<T>::staticLinearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        T::staticLinearCast(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        T::staticLinearCast(b, a, &flippedInput, &flipped, 0);
    }
}

template struct hkSymmetricAgentLinearCast<hkBvAgent>;

void hkSymmetricBvAgent::linearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkBvAgent::linearCast(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        hkBvAgent::linearCast(b, a, &flippedInput, &flipped, 0);
    }
}

namespace {

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
} // namespace

// Stand-in for the dispatcher registration function (not recovered yet; takes the dispatcher as first argument).
// Stand-ins for the linear cast statics of the symmetric and plain BV agents (not written yet).

// Registers the BV agent for the pair of shape types in both orders: the symmetric variant for
// (any, 0x16) and the plain variant for (0x16, any).
void hkBvAgent::registerAgent(void* dispatcher) {
    hkAgentFuncs symmetric;
    symmetric.create = (hkAgentFunc)createShapeBvAgent;
    symmetric.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast<hkBvAgent>::staticGetPenetrations;
    symmetric.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast<hkBvAgent>::staticGetClosestPoints;
    symmetric.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast<hkBvAgent>::staticLinearCast;
    symmetric.symmetricA = true;
    symmetric.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&symmetric, -1, 0x16);

    hkAgentFuncs plain;
    plain.create = (hkAgentFunc)createBvShapeAgent;
    plain.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    plain.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    plain.staticLinearCast = (hkAgentFunc)staticLinearCast;
    plain.symmetricA = false;
    plain.symmetricB = true;
    ((hkCollisionDispatcher*)dispatcher)->registerCollisionAgent(&plain, 0x16, -1);
}
