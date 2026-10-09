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

// Stand-in for the transform composition used by updateShapeCollectionFilter (not recovered yet).
extern "C" void fn_802873E8(void* out, void* motion, void* shapeTransform);

// HYPOTHESIS: per-body record handed to the child agent. Layout from the stack copy: 0x00 shape field
// 0x10, 0x04 shape key, 0x08 pointer to the composed transform, 0x0C the body itself.
struct hkTransformAgentBodyInfo {
    void* unk00;    // HYPOTHESIS: shape field at 0x10
    u32 shapeKey;   // 0x04
    void* transform; // 0x08
    hkCdBody* body; // 0x0C
};

void hkTransformAgent::updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2) {
    hkCdBody* body = (hkCdBody*)unk0;
    char* shape = (char*)body->m_shape;
    hkTransform transform;
    fn_802873E8(&transform, body->m_motion, shape + 0x30);
    hkTransformAgentBodyInfo info;
    info.body = body;
    info.shapeKey = body->m_shapeKey;
    info.transform = &transform;
    info.unk00 = *(void**)(shape + 0x10);
    m_childAgent->updateShapeCollectionFilterChild(&info, unk1, unk2);
}

void hkTransformAgent::cleanup() {
    m_childAgent->cleanupChild();
    delete this;
}

hkTransformAgent::~hkTransformAgent() {}

void hkTransformAgent::invalidateTim() {
    m_childAgent->invalidateTimChild();
}

void hkTransformAgent::warpTime() {
    m_childAgent->warpTimeChild();
}

void hkTransformAgent::removePoint() {
    m_childAgent->removePointChild();
}

void hkTransformAgent::commitPotential() {
    m_childAgent->commitPotentialChild();
}

void hkTransformAgent::createZombie() {
    m_childAgent->createZombieChild();
}

hkSymmetricAgent_16hkTransformAgent_::~hkSymmetricAgent_16hkTransformAgent_() {}

hkSymmetricAgentLinearCast_16hkTransformAgent_::~hkSymmetricAgentLinearCast_16hkTransformAgent_() {}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_16hkTransformAgent_::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkTransformAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkTransformAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkTransformAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkTransformAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_16hkTransformAgent_::updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2) {
    hkTransformAgent::updateShapeCollectionFilter(unk1, unk0, unk2);
}

void hkFlagCdBodyPairCollector::addCdBodyPair(hkCdBody* bodyA, hkCdBody* bodyB) {
    m_hit = 1;
}

void hkClosestCdPointCollector::addCdPoint(hkCdPoint* point) {
    if (m_lastChainA != 0 && !(point->data.f7 < m_point.f7)) {
        return;
    }
    m_point = point->data;
    hkCdPointChain* last;
    for (last = point->m_chainA; last->m_next != 0; last = last->m_next) {
    }
    m_lastChainA = last;
    m_idA = point->m_chainA->m_id;
    for (last = point->m_chainB; last->m_next != 0; last = last->m_next) {
    }
    m_lastChainB = last;
    m_idB = point->m_chainB->m_id;
    m_dist = point->data.f7;
}

void hkSimpleClosestContactCollector::addCdPoint(hkCdPoint* point) {
    if (m_hit && !(point->data.f7 < m_point.f7)) {
        return;
    }
    m_point = point->data;
    m_hit = hkBool(true);
    m_dist = point->data.f7;
}
