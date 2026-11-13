// Havok translation unit hkTransformAgent.o (main.dol 0x802C8E9C-0x802CAA50).
// Functions in address order (method names from the Havok TU map):
//   0x802C8E9C   204  registerAgent   [map: hkTransformAgent__registerAgent]
//   0x802C8F68    92  cleanup   [map: hkTransformAgent__cleanup]
//   0x802C8FC4    92  __dt   [map: hkTransformAgent____dt]
//   0x802C9020   832  createTransformAAgent   [map: hkTransformAgent__createTransformAAgent]
//   0x802C9360   856  createTransformBAgent   [map: hkTransformAgent__createTransformBAgent]
//   0x802C96B8    92  __dt   [map: hkSymmetricAgentLinearCast_16hkTransformAgent_____dt]
//   0x802C9714   888  processCollision   [map: hkTransformAgent__processCollision]
//   0x802C9A8C   304  linearCast   [map: hkTransformAgent__linearCast]
//   0x802C9BBC   360  staticLinearCast   [map: hkTransformAgent__staticLinearCast]
//   0x802C9D24   296  getClosestPoints   [map: hkTransformAgent__getClosestPoints]
//   0x802C9E4C   352  staticGetClosestPoints   [map: hkTransformAgent__staticGetClosestPoints]
//   0x802C9FAC   296  getPenetrations   [map: hkTransformAgent__getPenetrations]
//   0x802CA0D4   352  staticGetPenetrations   [map: hkTransformAgent__staticGetPenetrations]
//   0x802CA234   148  updateShapeCollectionFilter   [map: hkTransformAgent__updateShapeCollectionFilter]
//   0x802CA2C8    20  invalidateTim   [map: hkTransformAgent__invalidateTim]
//   0x802CA2DC    20  warpTime   [map: hkTransformAgent__warpTime]
//   0x802CA2F0    20  removePoint   [map: hkTransformAgent__removePoint]
//   0x802CA304    20  commitPotential   [map: hkTransformAgent__commitPotential]
//   0x802CA318    20  createZombie   [map: hkTransformAgent__createZombie]
//   0x802CA32C   352  linearCast   [map: hkSymmetricAgent_16hkTransformAgent___linearCast]
//   0x802CA48C    72  getPenetrations   [map: hkSymmetricAgentLinearCast_16hkTransformAgent___getPenetrations]
//   0x802CA4D4    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_16hkTransformAgent___staticGetPenetrations]
//   0x802CA51C    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_16hkTransformAgent___getClosestPoints]
//   0x802CA564    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_16hkTransformAgent___staticGetClosestPoints]
//   0x802CA5AC   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_16hkTransformAgent___staticLinearCast]
//   0x802CA70C   412  processCollision   [map: hkSymmetricAgentLinearCast_16hkTransformAgent___processCollision]
//   0x802CA8A8    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_16hkTransformAgent___updateShapeCollectionFilter]
//   0x802CA8B8    92  __dt   [map: hkSymmetricAgent_16hkTransformAgent_____dt]
//   0x802CA914    12  addCdBodyPair   [map: hkFlagCdBodyPairCollector__addCdBodyPair]
//   0x802CA920   184  addCdPoint   [map: hkClosestCdPointCollector__addCdPoint]
//   0x802CA9D8   120  addCdPoint   [map: hkSimpleClosestContactCollector__addCdPoint]

#include <havok/hkTransformAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkTransform.h>
#include <havok/hkFlagCdBodyPairCollector.h>
#include <havok/hkClosestCdPointCollector.h>
#include <havok/hkSimpleClosestContactCollector.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCdBody.h>
#include <havok/hkMotionState.h>

void hkTransformAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs symmetric;
    symmetric.create = (hkAgentFunc)createTransformBAgent;
    symmetric.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetPenetrations;
    symmetric.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetClosestPoints;
    symmetric.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_16hkTransformAgent_::staticLinearCast;
    symmetric.symmetricA = 1;
    symmetric.symmetricB = 1;
    dispatcher->registerCollisionAgent(&symmetric, -1, HK_SHAPE_TRANSFORM);

    hkAgentFuncs plain;
    plain.create = (hkAgentFunc)createTransformAAgent;
    plain.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    plain.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    plain.staticLinearCast = (hkAgentFunc)staticLinearCast;
    plain.symmetricA = 0;
    plain.symmetricB = 1;
    dispatcher->registerCollisionAgent(&plain, HK_SHAPE_TRANSFORM, -1);
}

// HYPOTHESIS: the motion state assignment of the constructor copies the transform through one hkTransform temporary
// (the target keeps its dead stack copy just below the motion state), then the rest member by member. Kept local to this
// unit like hkMultiSphereAgent_copyMotionState (that unit's temporaries are four vectors instead).
static inline void hkTransformAgent_copyMotionState(hkMotionState& dst, const hkMotionState& src) {
    hkTransform transform = src.m_transform;
    dst.m_transform = transform;
    dst.m_sweptTransform = src.m_sweptTransform;
    dst.m_deltaAngle = src.m_deltaAngle;
    dst.m_objectRadius = src.m_objectRadius;
    dst.m_maxLinearVelocity = src.m_maxLinearVelocity;
    dst.m_maxAngularVelocity = src.m_maxAngularVelocity;
    dst.m_linearDamping = src.m_linearDamping;
    dst.m_angularDamping = src.m_angularDamping;
    dst.m_deactivationClass = src.m_deactivationClass;
    dst.m_deactivationCounter = src.m_deactivationCounter;
}

// The child body: A's motion state moved by the shape's transform, the shape's child shape and A's key.
// The hkSimdReal motion state scalar (see hkMotionState.h) puts this function in the mode that keeps the hkCollisionAgent
// vtable store and the dead transform temporary, as in the target.
// The `mr r30, r31` copy in the create functions is the object pointer kept for the exception cleanup of the base (the
// unit is built with -Cpp_exceptions on).
hkTransformAgent::hkTransformAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                   hkContactMgr* mgr)
    : hkCollisionAgent((int)mgr) {
    const hkTransformShape* shape = (const hkTransformShape*)bodyA.m_shape;
    hkMotionState motion;
    hkMotionState* childMotion = &motion;
    const hkShape* child = shape->m_childShape.m_childShape;
    hkTransformAgent_copyMotionState(motion, *(const hkMotionState*)bodyA.m_motion);
    motion.m_transform.setMul(*(const hkTransform*)bodyA.m_motion, shape->m_transform);
    hkCdBody childBody(&bodyA, childMotion);
    childBody.m_shapeKey = bodyA.m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    m_childAgent = input.m_dispatcher->getNewCollisionAgent(childBody, bodyB, input, mgr);
}

hkTransformAgent* hkTransformAgent::createTransformAAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                         hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkTransformAgent(bodyA, bodyB, input, contactMgr);
}

hkTransformAgent* hkTransformAgent::createTransformBAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                         hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_16hkTransformAgent_(bodyA, bodyB, input, contactMgr);
}

// The child gets a full motion state: A's motion state moved by the shape's transform (the swept rotations turned by
// the shape's rotation, the local center of mass expressed in the child's frame).
// NONMATCHING (~53%): structure, timer form and stack layout match; the quaternion products are scheduled differently
// (the target needs one more FPR, f31).
void hkTransformAgent::processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d) {
    HK_TIMER_BEGIN("Transform");
    const hkCdBody* bodyA = &a;
    const hkTransformShape* shape = (const hkTransformShape*)bodyA->m_shape;
    hkMotionState motion;
    motion.m_transform.setMul(*(const hkTransform*)bodyA->m_motion, shape->m_transform);
    const hkMotionState* src = (const hkMotionState*)bodyA->m_motion;
    motion.m_sweptTransform.m_centerOfMass0 = src->m_sweptTransform.m_centerOfMass0;
    motion.m_sweptTransform.m_centerOfMass1 = src->m_sweptTransform.m_centerOfMass1;
    motion.m_sweptTransform.m_rotation0.setMul(src->m_sweptTransform.m_rotation0, shape->m_rotation);
    motion.m_sweptTransform.m_rotation1.setMul(src->m_sweptTransform.m_rotation1, shape->m_rotation);
    motion.m_sweptTransform.m_centerOfMassLocal._setTransformedInversePos(shape->m_transform,
                                                                          src->m_sweptTransform.m_centerOfMassLocal);
    motion.m_deltaAngle = src->m_deltaAngle;
    motion.m_objectRadius = src->m_objectRadius;
    hkCdBody childBody(bodyA, &motion);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA->m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    m_childAgent->processCollision(childBody, b, c, d);
    HK_TIMER_END();
}

// Every query hands the child agent (or, for the statics, the dispatcher's function for the child shape against B) a
// child body: the transform shape's child, A's shape key, and A's transform composed with the shape's transform.
// All of them are exact.
void hkTransformAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    HK_TIMER_BEGIN("Transform");
    const hkCdBody* bodyA = &a;
    const hkTransformShape* shape = (const hkTransformShape*)bodyA->m_shape;
    hkTransform transform;
    transform.setMul(*(const hkTransform*)bodyA->m_motion, shape->m_transform);
    hkCdBody childBody(bodyA, &transform);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA->m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    // The timer ends before the child call here (unlike the other queries).
    HK_TIMER_END();
    m_childAgent->getPenetrations(childBody, b, c, target);
}

void hkTransformAgent::staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                             void* target) {
    HK_TIMER_BEGIN("Transform");
    const hkTransformShape* shape = (const hkTransformShape*)bodyA.m_shape;
    hkTransform transform;
    transform.setMul(*(const hkTransform*)bodyA.m_motion, shape->m_transform);
    hkCdBody childBody(&bodyA, &transform);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA.m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    int typeA = child->getType();
    int typeB = bodyB.m_shape->getType();
    typedef void (*QueryFunc)(void*, void*, void*, void*);
    ((QueryFunc)input.m_dispatcher->getGetPenetrationsFunc(typeA, typeB))(
        &childBody, (void*)&bodyB, (void*)&input, target);
    HK_TIMER_END();
}

void hkTransformAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    HK_TIMER_BEGIN("Transform");
    const hkCdBody* bodyA = &a;
    const hkTransformShape* shape = (const hkTransformShape*)bodyA->m_shape;
    hkTransform transform;
    transform.setMul(*(const hkTransform*)bodyA->m_motion, shape->m_transform);
    hkCdBody childBody(bodyA, &transform);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA->m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    m_childAgent->getClosestPoints(childBody, b, c, target);
    HK_TIMER_END();
}

void hkTransformAgent::staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                              void* target) {
    HK_TIMER_BEGIN("Transform");
    const hkTransformShape* shape = (const hkTransformShape*)bodyA.m_shape;
    hkTransform transform;
    transform.setMul(*(const hkTransform*)bodyA.m_motion, shape->m_transform);
    hkCdBody childBody(&bodyA, &transform);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA.m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    int typeA = child->getType();
    int typeB = bodyB.m_shape->getType();
    typedef void (*QueryFunc)(void*, void*, void*, void*);
    ((QueryFunc)input.m_dispatcher->getGetClosestPointsFunc(typeA, typeB))(
        &childBody, (void*)&bodyB, (void*)&input, target);
    HK_TIMER_END();
}

void hkTransformAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    HK_TIMER_BEGIN("Transform");
    const hkCdBody* bodyA = &a;
    const hkTransformShape* shape = (const hkTransformShape*)bodyA->m_shape;
    hkTransform transform;
    transform.setMul(*(const hkTransform*)bodyA->m_motion, shape->m_transform);
    hkCdBody childBody(bodyA, &transform);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA->m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    m_childAgent->linearCast(childBody, b, c, target, d);
    HK_TIMER_END();
}

void hkTransformAgent::staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                        void* target, void* d) {
    HK_TIMER_BEGIN("Transform");
    const hkTransformShape* shape = (const hkTransformShape*)bodyA.m_shape;
    hkTransform transform;
    transform.setMul(*(const hkTransform*)bodyA.m_motion, shape->m_transform);
    hkCdBody childBody(&bodyA, &transform);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA.m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    int typeA = child->getType();
    int typeB = bodyB.m_shape->getType();
    typedef void (*CastFunc)(void*, void*, void*, void*, void*);
    ((CastFunc)input.m_dispatcher->getLinearCastFunc(typeA, typeB))(
        &childBody, (void*)&bodyB, (void*)&input, target, d);
    HK_TIMER_END();
}

// BYTES: the symmetric wrapper calls this function, it does not inline it.
#pragma auto_inline off
void hkTransformAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
    const hkCdBody* bodyA = &a;
    const hkTransformShape* shape = (const hkTransformShape*)bodyA->m_shape;
    hkTransform transform;
    transform.setMul(*(const hkTransform*)bodyA->m_motion, shape->m_transform);
    hkCdBody childBody(bodyA, &transform);
    const hkShape* child = shape->m_childShape.m_childShape;
    childBody.m_shapeKey = bodyA->m_shapeKey;
    childBody.m_shape = (hkShape*)child;
    m_childAgent->updateShapeCollectionFilter(childBody, b, c);
}
#pragma auto_inline reset

void hkTransformAgent::cleanup() {
    m_childAgent->cleanup();
    delete this;
}

hkTransformAgent::~hkTransformAgent() {}

void hkTransformAgent::invalidateTim(void* arg) {
    m_childAgent->invalidateTim(arg);
}

void hkTransformAgent::warpTime(float t0, float t1, void* arg) {
    m_childAgent->warpTime(t0, t1, arg);
}

void hkTransformAgent::removePoint(u16 key) {
    m_childAgent->removePoint(key);
}

void hkTransformAgent::commitPotential(u16 key) {
    m_childAgent->commitPotential(key);
}

void hkTransformAgent::createZombie(u16 key) {
    m_childAgent->createZombie(key);
}

hkSymmetricAgent_16hkTransformAgent_::~hkSymmetricAgent_16hkTransformAgent_() {}

hkSymmetricAgentLinearCast_16hkTransformAgent_::~hkSymmetricAgentLinearCast_16hkTransformAgent_() {}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_16hkTransformAgent_::getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkTransformAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkTransformAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkTransformAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkTransformAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::updateShapeCollectionFilter(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2) {
    hkTransformAgent::updateShapeCollectionFilter(unk1, unk0, unk2);
}

void hkFlagCdBodyPairCollector::addCdBodyPair(hkCdBody* bodyA, hkCdBody* bodyB) {
    m_hit = 1;
}

void hkClosestCdPointCollector::addCdPoint(hkCdPoint* point) {
    if (m_rootA != 0 && !(point->m_contact.m_normal.w < m_point.m_normal.w)) {
        return;
    }
    m_point = point->m_contact;
    const hkCdBody* root;
    for (root = point->m_cdBodyA; root->m_parent != 0; root = root->m_parent) {
    }
    m_rootA = root;
    m_shapeKeyA = point->m_cdBodyA->m_shapeKey;
    for (root = point->m_cdBodyB; root->m_parent != 0; root = root->m_parent) {
    }
    m_rootB = root;
    m_shapeKeyB = point->m_cdBodyB->m_shapeKey;
    m_earlyOutDistance = point->m_contact.m_normal.w;
}

void hkSimpleClosestContactCollector::addCdPoint(hkCdPoint* point) {
    if (m_hit && !(point->m_contact.m_normal.w < m_point.m_normal.w)) {
        return;
    }
    m_point = point->m_contact;
    m_hit = hkBool(true);
    m_dist = point->m_contact.m_normal.w;
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_16hkTransformAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkTransformAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkTransformAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkTransformAgent::staticLinearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkTransformAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d) {
    hkSymmetricAgent_processCollision<hkTransformAgent>(this, a, b, c, d);
}
