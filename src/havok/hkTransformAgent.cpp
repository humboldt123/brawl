// Havok translation unit hkTransformAgent.o (main.dol 0x802C8E9C-0x802CAA50).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
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


namespace {

// Layout of the table handed to the dispatcher (stack copy in registerAgent).
} // namespace

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

// HYPOTHESIS: per-body record handed to the child agent. Layout from the stack copy: 0x00 shape field
// 0x10, 0x04 shape key, 0x08 pointer to the composed transform, 0x0C the body itself.
struct hkTransformAgentBodyInfo {
    void* unk00;    // HYPOTHESIS: shape field at 0x10
    u32 shapeKey;   // 0x04
    void* transform; // 0x08
    hkCdBody* body; // 0x0C
};

#pragma dont_inline on
void hkTransformAgent::updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2) {
    hkCdBody* body = (hkCdBody*)unk0;
    char* shape = (char*)body->m_shape;
    hkTransform transform;
    // body's transform composed with the transform shape's own transform (at 0x30 in the shape)
    transform.setMul(*(const hkTransform*)body->m_motion, *(const hkTransform*)(shape + 0x30));
    hkTransformAgentBodyInfo info;
    info.body = body;
    info.shapeKey = body->m_shapeKey;
    info.transform = &transform;
    info.unk00 = *(void**)(shape + 0x10);
    m_childAgent->updateShapeCollectionFilterChild(&info, unk1, unk2);
}
#pragma dont_inline reset

void hkTransformAgent::cleanup() {
    m_childAgent->cleanupChild();
    delete this;
}

hkTransformAgent::~hkTransformAgent() {}

void hkTransformAgent::invalidateTim(void* arg) {
    m_childAgent->invalidateTimChild(arg);
}

void hkTransformAgent::warpTime(float t0, float t1, void* arg) {
    m_childAgent->warpTimeChild(t0, t1, arg);
}

void hkTransformAgent::removePoint(u16 key) {
    m_childAgent->removePointChild(key);
}

void hkTransformAgent::commitPotential(u16 key) {
    m_childAgent->commitPotentialChild(key);
}

void hkTransformAgent::createZombie(u16 key) {
    m_childAgent->createZombieChild(key);
}

hkSymmetricAgent_16hkTransformAgent_::~hkSymmetricAgent_16hkTransformAgent_() {}

hkSymmetricAgentLinearCast_16hkTransformAgent_::~hkSymmetricAgentLinearCast_16hkTransformAgent_() {}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_16hkTransformAgent_::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkTransformAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkTransformAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkTransformAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkTransformAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2) {
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
    m_dist = point->m_contact.m_normal.w;
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
void hkSymmetricAgent_16hkTransformAgent_::linearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkTransformAgent::linearCast(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        hkTransformAgent::linearCast(b, a, &flippedInput, &flipped, 0);
    }
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticLinearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkTransformAgent::staticLinearCast(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        hkTransformAgent::staticLinearCast(b, a, &flippedInput, &flipped, 0);
    }
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::processCollision(void* a, void* b, void* c, void* d) {
    hkSymmetricAgent_processCollision<hkTransformAgent>(this, a, b, c, d);
}
