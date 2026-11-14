// Havok translation unit hkSimpleConstraintContactMgr.o (main.dol 0x802D8064-0x802D94A8).
// Functions in address order (names from the vtable slots; HYPOTHESIS where the slot meaning is inferred):
//   0x802D8064     8  getConstraintInstance (weak)
//   0x802D806C   220  __ct
//   0x802D8148   188  hkSimpleContactConstraintData::__dt (weak)
//   0x802D8204   264  __dt
//   0x802D830C    52  reserveContactPoints
//   0x802D8340   476  addToi (not written)
//   0x802D851C   192  removeToi (not written)
//   0x802D85DC   352  processToi (not written)
//   0x802D873C  1440  addContactPoint (not written)
//   0x802D8CDC   336  removeContactPoint (not written)
//   0x802D8E2C    32  getContactPoint
//   0x802D8E4C    44  getContactPointProperties
//   0x802D8E78   168  getAllContactPointIds
//   0x802D8F20    16  toiCollisionResponseBeginCallback (weak)
//   0x802D8F30    16  toiCollisionResponseEndCallback (weak)
//   0x802D8F40   880  processContact (not written)
//   0x802D92B0    28  Factory::__ct
//   0x802D92CC   132  Factory::createContactMgr
//   0x802D9350   212  calcStatistics
//   0x802D9424     8  getType (weak)
//   0x802D942C    32  cleanup (weak)
//   0x802D944C    92  Factory::__dt (weak)

#include <havok/hkSimpleConstraintContactMgr.h>
#include <havok/hkWorld.h>
#include <havok/hkRigidBody.h>
#include <havok/hkCollidable.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkEntityCallbackUtil.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkMath.h>
#include <havok/hkVector4.h>

hkSimpleConstraintContactMgr::hkSimpleConstraintContactMgr(hkWorld* world, hkRigidBody* bodyA, hkRigidBody* bodyB)
    : m_contactConstraintData(&m_constraint), m_constraint(hkConstraintInstance::PRIORITY_PSI) {
    m_constraint.m_data = &m_contactConstraintData;
    m_constraint.m_entities[0] = bodyA;
    m_constraint.m_entities[1] = bodyB;
    m_world = world;
    m_skipNextNprocessCallbacks = 0;
    m_reservedContactPoints = 0;
    m_contactConstraintData.m_memSizeAndFlags = 0;
    m_constraint.m_memSizeAndFlags = 0;
    hkCollisionDispatcher* dispatcher = world->m_collisionDispatcher;
    int index = dispatcher->getCollisionQualityIndex(bodyA->m_collidable.m_broadPhaseHandle.m_objectQualityType,
                                                     bodyB->m_collidable.m_broadPhaseHandle.m_objectQualityType);
    m_constraint.setPriority(
        (hkConstraintInstance::ConstraintPriority)dispatcher->getCollisionQualityInfo(index)->unk38);
}

hkSimpleConstraintContactMgr::~hkSimpleConstraintContactMgr() {
    if (m_contactConstraintData.m_atom->m_numContactPoints != 0) {
        hkWorldOperationUtil::removeConstraintImmediately(m_world, &m_constraint, true);
    }
    m_constraint.m_entities[0] = 0;
    m_constraint.m_entities[1] = 0;
    m_constraint.m_data = 0;
}

hkResult hkSimpleConstraintContactMgr::reserveContactPoints(int numPoints) {
    int reserved = m_reservedContactPoints + numPoints;
    if (m_contactConstraintData.m_atom->m_numContactPoints + reserved > 0xFE) {
        return 1;
    }
    m_reservedContactPoints = (u8)reserved;
    return 0;
}

hkContactPoint* hkSimpleConstraintContactMgr::getContactPoint(hkContactPointId id) {
    return &m_contactConstraintData.m_atom->getContactPoints()[m_contactConstraintData.m_idMgrA.m_values[id]];
}

hkContactPointProperties* hkSimpleConstraintContactMgr::getContactPointProperties(hkContactPointId id) {
    return &m_contactConstraintData.m_atom->getContactPointProperties()[m_contactConstraintData.m_idMgrA.m_values[id]];
}

void hkSimpleConstraintContactMgr::getAllContactPointIds(hkArray<hkContactPointId>& contactPointIds) const {
    for (int i = 0; i < m_contactConstraintData.m_idMgrA.m_values.m_size; i++) {
        if (m_contactConstraintData.m_idMgrA.m_values[i] != 0xFF) {
            contactPointIds.pushBack(i);
        }
    }
}

hkSimpleConstraintContactMgr::Factory::Factory(hkWorld* world) {
    m_world = world;
}

hkContactMgr* hkSimpleConstraintContactMgr::Factory::createContactMgr(const hkCollidable& a, const hkCollidable& b,
                                                                      const hkCollisionInput& input) {
    hkRigidBody* bodyA = (hkRigidBody*)((const char*)&a + a.m_ownerOffset);
    hkRigidBody* bodyB = (hkRigidBody*)((const char*)&b + b.m_ownerOffset);
    return new hkSimpleConstraintContactMgr(m_world, bodyA, bodyB);
}

// Statistics collector interface as called here (slot offsets from the calls).
struct hkSimpleConstraintContactMgrStatisticsCollector {
    virtual void unk08();
    virtual void beginObject(const char* name, int type, const void* object);                    // 0x0C
    virtual void addArray(const char* name, int type, const void* data, int size, int capacity); // 0x10
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void endObject();                                                                     // 0x20
};

void hkSimpleConstraintContactMgr::calcStatistics(hkStatisticsCollector* collector) const {
    hkSimpleConstraintContactMgrStatisticsCollector* c = (hkSimpleConstraintContactMgrStatisticsCollector*)collector;
    c->beginObject(0, 8, this);
    c->addArray("ContactAtom", 8, m_contactConstraintData.m_atom, m_contactConstraintData.m_atom->m_sizeOfAllAtoms, 0);
    const char* idsName = "ContactIds";
    if (m_contactConstraintData.m_idMgrA.m_values.mustDeallocate()) {
        c->addArray(idsName, 8, m_contactConstraintData.m_idMgrA.m_values.m_data, m_contactConstraintData.m_idMgrA.m_values.m_size,
                    m_contactConstraintData.m_idMgrA.m_values.getCapacity());
    }
    c->endObject();
}

// Event handed to the world and entity contact-removed callbacks (0x18 bytes on the stack). HYPOTHESIS: layout
// beyond the stores made here; the callback symbols type it as ContactPointEvent / hkCollisionEventRemoved.
struct hkSimpleContactPointRemovedEvent {
    hkContactPointId m_contactPointId;      // 0x00
    hkContactPointMaterial* m_material;     // 0x04
    hkEntity* m_entityA;                    // 0x08
    hkEntity* m_entityB;                    // 0x0C
    void* unk10;                            // 0x10
    hkContactMgr* m_contactMgr;             // 0x14

    hkSimpleContactPointRemovedEvent(hkContactPointId id, hkContactPointMaterial* material, hkEntity* a, hkEntity* b,
                                     hkContactMgr* mgr)
        : m_contactPointId(id), m_material(material), m_entityA(a), m_entityB(b), m_contactMgr(mgr) {}
};

void hkSimpleConstraintContactMgr::removeContactPoint(u16 contactPointId) {
    hkContactPointProperties* props =
        &m_contactConstraintData.m_atom->getContactPointProperties()[m_contactConstraintData.m_idMgrA.m_values[contactPointId]];
    hkEntity* entityA = m_constraint.m_entities[0];
    hkEntity* entityB = m_constraint.m_entities[1];
    hkSimpleContactPointRemovedEvent event(contactPointId, props, entityA, entityB, this);
    if (contactPointId != 0xFFFF) {
        event.m_material = getContactPointProperties(contactPointId);
    }
    hkWorldCallbackUtil::fireContactPointRemoved(m_world, (hkWorldCallbackUtil::ContactPointEvent*)&event);
    if (entityA->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointRemovedInternal(entityA, (hkCollisionEventRemoved*)&event);
    }
    if (entityB->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointRemovedInternal(entityB, (hkCollisionEventRemoved*)&event);
    }

    m_contactConstraintData.freeContactPoint(contactPointId);
    if (m_contactConstraintData.m_atom->m_numContactPoints == 0) {
        if (m_world->m_unk9C == 0) {
            hkWorldOperationUtil::removeConstraintFromCriticalLockedIsland(m_world, &m_constraint);
        } else {
            entityA->m_simulationIsland->markForWrite();
            entityB->m_simulationIsland->markForWrite();
            hkWorldOperationUtil::removeConstraintFromCriticalLockedIsland(m_world, &m_constraint);
        }
    }
}

// Entity owning a collidable, or 0 for other owner types (broad phase type 1 = entity). HYPOTHESIS: name.
static inline hkEntity* hkGetEntityOfCollidable(const hkCollidable& c) {
    if (c.m_broadPhaseHandle.m_type == 1) {
        return (hkEntity*)((const char*)&c + c.m_ownerOffset);
    }
    return 0;
}

void hkSimpleConstraintContactMgr::removeToi(const hkCollidable& a, const hkCollidable& b,
                                             hkContactPointMaterial& material) {
    hkEntity* entityA = hkGetEntityOfCollidable(a);
    hkEntity* entityB = hkGetEntityOfCollidable(b);
    hkWorld* world = entityA->m_world;
    hkSimpleContactPointRemovedEvent event(0xFFFF, &material, entityA, entityB, this);
    hkWorldCallbackUtil::fireContactPointRemoved(world, (hkWorldCallbackUtil::ContactPointEvent*)&event);
    if (entityA->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointRemovedInternal(entityA, (hkCollisionEventRemoved*)&event);
    }
    if (entityB->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointRemovedInternal(entityB, (hkCollisionEventRemoved*)&event);
    }
}

// Event handed to the contact-confirmed callbacks (0x24 bytes on the stack). HYPOTHESIS: layout beyond the stores.
struct hkSimpleContactPointConfirmedEvent {
    const hkCollidable* m_collidableA;     // 0x00
    const hkCollidable* m_collidableB;     // 0x04
    void* unk08;                           // 0x08
    const hkContactPoint* m_contactPoint;  // 0x0C
    hkContactPointMaterial* m_material;    // 0x10
    hkReal m_rotateNormal;                 // 0x14
    hkReal m_projectedVelocity;            // 0x18
    void* unk1C;                           // 0x1C
    void* unk20;                           // 0x20

    hkSimpleContactPointConfirmedEvent(const hkCollidable* a, const hkCollidable* b, const hkContactPoint* cp,
                                       hkContactPointMaterial* material, hkReal rotateNormal, hkReal projectedVelocity)
        : m_collidableA(a), m_collidableB(b), m_contactPoint(cp), m_material(material), m_rotateNormal(rotateNormal),
          m_projectedVelocity(projectedVelocity), unk1C(0), unk20(0) {}
};

// View of the continuous simulation's toi record as read here (0x40 bytes). HYPOTHESIS: names.
struct hkSimpleToiEventView {
    hkReal m_time;                       // 0x00
    hkEntity* m_entities[2];             // 0x04
    hkReal m_seperatingVelocity;         // 0x0C
    hkContactPointMaterial m_material;   // 0x10
    void* unk18;                         // 0x18
    hkContactPoint m_contactPoint;       // 0x20
};

// hkWorld simple collision response (hkContinuousSimulation.o, map name hkLs_doSimpleCollisionResponse).
// HYPOTHESIS: parameters.
extern "C" void fn_80328E50(hkWorld* world, const void* toiEvent, hkReal rotateNormal, void* entitiesOut);

static inline hkBool isKeyframedMotion(hkEntity* entity) {
    return entity->getMotion()->m_type == hkMotion::MOTION_KEYFRAMED;
}

static inline hkBool isFixedOrKeyframed(hkEntity* entity) {
    return entity->isFixed() || isKeyframedMotion(entity);
}

static inline bool needsCollisionResponse(hkEntity* a, hkEntity* b) {
    bool respond = true;
    if (isFixedOrKeyframed(a)) {
        if (isFixedOrKeyframed(b)) {
            respond = false;
        }
    }
    return respond;
}

void hkSimpleConstraintContactMgr::processToi(const void* toiEvent, hkReal rotateNormal, void* entitiesOut) {
    const hkSimpleToiEventView& ev = *(const hkSimpleToiEventView*)toiEvent;
    hkEntity* a = ev.m_entities[0];
    hkEntity* b = ev.m_entities[1];
    hkWorld* world = a->m_world;
    hkSimpleContactPointConfirmedEvent event(&a->m_collidable, &b->m_collidable,
                                             &ev.m_contactPoint, (hkContactPointMaterial*)&ev.m_material, rotateNormal,
                                             ev.m_seperatingVelocity);
    hkWorldCallbackUtil::fireContactPointConfirmed(world, (hkWorldCallbackUtil::ContactPointEvent*)&event);
    if (ev.m_entities[0]->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointConfirmedInternal(ev.m_entities[0], (hkCollisionEventConfirmed*)&event);
    }
    if (ev.m_entities[1]->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointConfirmedInternal(ev.m_entities[1], (hkCollisionEventConfirmed*)&event);
    }
    hkEntity* entityA = ev.m_entities[0];
    hkReal rotate = event.m_rotateNormal;
    hkEntity* entityB = ev.m_entities[1];
    if (needsCollisionResponse(entityA, entityB)) {
        fn_80328E50(world, toiEvent, rotate, entitiesOut);
    }
}

// Square root as 1 / (refined reciprocal square root estimate) (0 for x <= 0). HYPOTHESIS: the inline's shape.
static inline hkReal hkSimpleSqrt(hkReal x) {
    hkReal inv;
    if (x <= 0.0f) {
        u32 bits = 0x7f800000;
        inv = *(hkReal*)&bits;
    } else {
        double estimate = __frsqrte(x);
        hkReal r = (hkReal)estimate;
        inv = (0.5f * r) * -(r * (x * r) - 3.0f);
    }
    return 1.0f / inv;
}

static inline hkEntity* hkGetEntityOfBody(const hkCdBody& body) {
    const hkCdBody* root = body.getRootCollidable();
    return (hkEntity*)((const char*)root + ((const hkCollidable*)root)->m_ownerOffset);
}

int hkSimpleConstraintContactMgr::addToi(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& input,
                                         hkProcessCollisionOutput& output, hkReal toi, hkContactPoint& cp,
                                         hkReal& projectedVelocity, hkContactPointMaterial& materialOut) {
    hkEntity* entityA = hkGetEntityOfBody(a);
    hkEntity* entityB = hkGetEntityOfBody(b);
    hkReal friction = hkSimpleSqrt(entityA->m_material.m_friction * entityB->m_material.m_friction);
    materialOut.m_friction = hkMath::hkFloatToInt(256.0f * friction);
    hkReal restitution = hkSimpleSqrt(entityA->m_material.m_restitution * entityB->m_material.m_restitution);
    materialOut.m_restitution = hkMath::hkFloatToInt(128.0f * restitution);

    hkContactPointAddedEvent event;
    event.m_bodyA = &a;
    event.m_bodyB = &b;
    event.unk08 = 0;
    event.m_output = &output;
    event.m_contactPoint = &cp;
    event.m_material = &materialOut;
    event.m_projectedVelocity = projectedVelocity;
    event.m_contactMgr = this;
    event.m_status = 0;
    event.m_toi = toi;
    hkWorldCallbackUtil::fireContactPointAdded(m_world, (hkWorldCallbackUtil::ContactPointEvent*)&event);
    if (entityA->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointAddedInternal(entityA, &event);
    }
    if (entityB->m_collisionListeners.m_size != 0) {
        hkEntityCallbackUtil::fireContactPointAddedInternal(entityB, &event);
    }
    projectedVelocity = event.m_projectedVelocity;
    return event.m_status;
}
