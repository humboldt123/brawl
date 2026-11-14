// Havok translation unit hkRigidBody.o (main.dol 0x802E22C4-0x802E3604).
// Functions in address order (method names from the Havok TU map):
//   0x802E22C4    76  finishLoadedObjecthkRigidBody   [map: hkRigidBody__finishLoadedObjecthkRigidBody]
//   0x802E2310    20  cleanupLoadedObjecthkRigidBody   [map: hkRigidBody__cleanupLoadedObjecthkRigidBody]
//   0x802E2324    84  getVtablehkRigidBody   [map: hkRigidBody__getVtablehkRigidBody]
//   0x802E2378    84  estimateAllowedPenetrationDepth   [map: hkCollidable__estimateAllowedPenetrationDepth]
//   0x802E23CC   476  updateCachedShapeInfo   [map: hkRigidBody__updateCachedShapeInfo]
//   0x802E25A8   664  createDynamicRigidMotion   [map: hkRigidBody__createDynamicRigidMotion]
//   0x802E2840   604  __ct   [map: hkRigidBody____ct]
//   0x802E2A9C   112  __dt   [map: hkRigidBody____dt]
//   0x802E2B0C   412  getCinfo   [map: hkRigidBody__getCinfo]
//   0x802E2CA8     8  getMotionState   [map: hkRigidBody__getMotionState]
//   0x802E2CB0     4  setDeactivator   [map: hkRigidBody__setDeactivator]
//   0x802E2CB4   836  clone   [map: hkRigidBody__clone]
//   0x802E2FF8    96  setMotionType   [map: hkRigidBody__setMotionType]
//   0x802E3058   464  setShape   [map: hkRigidBody__setShape]
//   0x802E3228    92  setCenterOfMassLocal   [map: hkRigidBody__setCenterOfMassLocal]
//   0x802E3284   268  setDeactivator   [map: hkRigidBody__setDeactivator1]
//   0x802E3390   328  updateBroadphaseAndResetCollisionInformationOfWarpedBody   [map: hkRigidBody__updateBroadphaseAndResetCollisionInformationOfWarpedBody]
//   0x802E34D8    68  setPosition   [map: hkRigidBody__setPosition]
//   0x802E351C    68  setPositionAndRotation   [map: hkRigidBody__setPositionAndRotation]
//   0x802E3560    68  setTransform   [map: hkRigidBody__setTransform]
//   0x802E35A4    16  setMass   [map: hkRigidBody__setMass]
//   0x802E35B4    80  __sinit_\hkRigidBody_cpp   [map: hkRigidBodycpp____sinit_]

#include <new>
#include <havok/hkRigidBody.h>
#include <havok/hkRegistry.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkFixedRigidMotion.h>
#include <havok/hkStabilizedBoxMotion.h>
#include <havok/hkThinBoxMotion.h>
#include <havok/hkBoxMotion.h>
#include <havok/hkStabilizedSphereMotion.h>
#include <havok/hkSphereMotion.h>
#include <havok/hkAabb.h>
#include <havok/hkEntityCallbackUtil.h>
#include <havok/hkRigidBodyDeactivator.h>
#include <havok/hkWorldOperationQueue.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkSimulation.h>

void hkRigidBody::finishLoadedObjecthkRigidBody(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    new (p) hkRigidBody(flag);
}

void hkRigidBody::cleanupLoadedObjecthkRigidBody(void* p) {
    ((hkRigidBody*)p)->~hkRigidBody();
}

#pragma push
#pragma auto_inline off
const void* hkRigidBody::getVtablehkRigidBody() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    hkVector4 buf[31]; // 0x1F0 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkRigidBody(flag);
    return *(const void**)buf;
}
#pragma pop

hkMotionState* hkRigidBody::getMotionState() {
    return &getMotion()->m_motionState;
}

#pragma push
#pragma auto_inline off // MATCH-ONLY: kept out of line for fn_802E3284, which calls it
void hkRigidBody::setDeactivator(hkEntityDeactivator* deactivator) {
    hkEntity::setDeactivator(deactivator);
}
#pragma pop

// After a teleport: the body's agents are reset and it is put through the broad phase again; an inactive body
// either wakes up or tells the world listeners that it moved.
void hkRigidBody::updateBroadphaseAndResetCollisionInformationOfWarpedBody() {
    hkWorld* world = m_world;
    if (world) {
        if (world->areCriticalOperationsLocked()) {
            hkWorldOperationRecord op;
            op.m_type = hkWorldOperation::UPDATE_MOVED_BODY_INFO;
            op.m_object = this;
            world->queueOperation(&op);
            return;
        }
        world->lockCriticalOperations();
        hkEntity* entity = this;
        world->m_simulation->resetCollisionInformationForEntities(&entity, 1, world, false);
        if (m_collidable.m_shape) {
            hkSimulation::collideEntitiesBroadPhaseDiscrete(&entity, 1, world);
        }
        if (!(bool)isActive()) {
            if ((bool)world->m_unkB6 && !(bool)isFixed()) {
                activate();
            }
            hkWorldCallbackUtil::fireInactiveEntityMoved(world, this);
        }
        world->unlockAndAttemptToExecutePendingOperations();
    }
}

void hkRigidBody::setPosition(const hkVector4& position) {
    getMotion()->setPosition(position);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

void hkRigidBody::setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation) {
    getMotion()->setPositionAndRotation(position, rotation);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

void hkRigidBody::setTransform(const hkTransform& transform) {
    getMotion()->setTransform(transform);
    updateBroadphaseAndResetCollisionInformationOfWarpedBody();
}

// While the world is locked the change is queued as an operation (kind 4); otherwise it is applied directly.
void hkRigidBody::setMotionType(u8 motionType, u8 a, u8 b) {
    if (m_world != 0 && m_world->m_lockCount != 0) {
        struct {
            u8 kind;            // 0x00 HYPOTHESIS: operation kind 4 (motion type change)
            u8 pad[3];
            hkRigidBody* body;  // 0x04
            u8 motionType;      // 0x08
            u8 a;               // 0x09
            u8 b;               // 0x0A
        } op;
        op.kind = 4;
        op.body = this;
        op.motionType = motionType;
        op.a = a;
        op.b = b;
        m_world->queueOperation(&op);
    } else {
        hkWorldOperationUtil::setRigidBodyMotionType(this, motionType, a, b);
    }
}

void hkRigidBody::setMass(hkReal mass) {
    getMotion()->setMass(mass);
}

void hkRigidBody::setCenterOfMassLocal(const hkVector4& com) {
    getMotion()->setCenterOfMassInLocal(com);
    hkVector4 info;
    updateCachedShapeInfo(m_collidable.m_shape, &info);
}

static hkStaticTypeInfo hkRigidBodyTypeInfo("hkRigidBody", hkRigidBody::finishLoadedObjecthkRigidBody, hkRigidBody::cleanupLoadedObjecthkRigidBody,
                                      hkRigidBody::getVtablehkRigidBody());

// Shape from https://github.com/jj-link/brawl src/havok/fn_802E2378.cpp.
// hkCollidable::estimateAllowedPenetrationDepth (inline copy): 0.2 * the smallest extent below 0.5, else 0.1.
// HYPOTHESIS: the unused float argument is the current depth (both callers pass it in f1).
// TODO: cleanup (shape from jj-link/brawl fn_802E2378)
#pragma push
#pragma auto_inline off
extern "C" void fn_802E2378(char* collidable, hkReal currentDepth, const float* extents) {
    float a = extents[1];
    float c = extents[2];
    float b = extents[0];
    b = (b < a) ? b : a;
    b = (b < c) ? b : c;
    if (b < 0.5f) {
        *(float*)(collidable + 0x20) = 0.2f * b;
        return;
    }
    *(float*)(collidable + 0x20) = 0.1f;
}
#pragma pop


// BYTES: the square root estimate goes through a double here (the hkVector4::length3 form gives other registers).
static inline hkReal hkRigidBody_length3(const hkVector4& v) {
    hkReal x = v.lengthSquared3();
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

// Caches the shape's extent and the body's bounding radius around the center of mass.
// BYTES: not an inline candidate (its three callers call it).
#pragma push
#pragma auto_inline off
void hkRigidBody::updateCachedShapeInfo(hkShape* shape, hkVector4* extentOut) {
    hkAabb aabb;
    shape->getAabb(hkTransform::getIdentity(), 0.0f, aabb);
    extentOut->setSub4(aabb.m_max, aabb.m_min);
    hkReal radius = 0.5f * hkRigidBody_length3(*extentOut);

    hkVector4 centerShapeSpace;
    centerShapeSpace.setAdd4(aabb.m_min, aabb.m_max);
    centerShapeSpace.mul4(0.5f);
    hkVector4 massCenterToAabbCenter;
    massCenterToAabbCenter.setSub4(centerShapeSpace, m_motion.m_motionState.m_sweptTransform.m_centerOfMassLocal);
    radius += hkRigidBody_length3(massCenterToAabbCenter);
    m_motion.m_motionState.m_objectRadius = radius;
}
#pragma pop

void hkRigidBody::createDynamicRigidMotion(hkMotion::MotionType motionType, const hkVector4& position,
                                           const hkQuaternion& rotation, hkReal mass, hkReal maxLinearVelocity,
                                           hkReal maxAngularVelocity, const hkMatrix3& inertiaLocal,
                                           const hkVector4& centerOfMassLocal, hkMotion* motionBuffer) {
    hkMotion* motion = motionBuffer;
    switch (motionType) {
    case hkMotion::MOTION_SPHERE_INERTIA:
        motion = new (motion) hkSphereMotion(position, rotation);
        break;
    case hkMotion::MOTION_BOX_INERTIA:
        motion = new (motion) hkBoxMotion(position, rotation);
        break;
    case hkMotion::MOTION_THIN_BOX_INERTIA:
        motion = new (motion) hkThinBoxMotion(position, rotation);
        break;
    case hkMotion::MOTION_STABILIZED_SPHERE_INERTIA:
        motion = new (motion) hkStabilizedSphereMotion(position, rotation);
        break;
    case hkMotion::MOTION_STABILIZED_BOX_INERTIA:
        motion = new (motion) hkStabilizedBoxMotion(position, rotation);
        break;
    case hkMotion::MOTION_DYNAMIC: {
        // Nearly isotropic inertia gives a sphere, a moderate spread a box, a large one a thin box.
        hkReal i0 = inertiaLocal.m_col0.x;
        hkReal i1 = inertiaLocal.m_col1.y;
        hkReal i2 = inertiaLocal.m_col2.z;
        hkReal maxI = i1 > i2 ? i1 : i2;
        maxI = i0 > maxI ? i0 : maxI;
        hkReal minI = i1 < i2 ? i1 : i2;
        minI = i0 < minI ? i0 : minI;
        if (minI > 0.8f * maxI) {
            motion = new (motion) hkSphereMotion(position, rotation);
        } else if (minI > 0.1f * maxI) {
            motion = new (motion) hkBoxMotion(position, rotation);
        } else {
            motion = new (motion) hkThinBoxMotion(position, rotation);
        }
        break;
    }
    case hkMotion::MOTION_KEYFRAMED:
        maxLinearVelocity = 1000000.0f;
        maxAngularVelocity = 1000000.0f;
        motion = new (motion) hkKeyframedRigidMotion(position, rotation);
        break;
    default:
        motion = new (motion) hkFixedRigidMotion(position, rotation);
        break;
    }

    if (motionType != hkMotion::MOTION_KEYFRAMED) {
        motion->setInertiaLocal(inertiaLocal);
        motion->setCenterOfMassInLocal(centerOfMassLocal);
        motion->setMass(mass);
    }
    motion->m_motionState.m_maxLinearVelocity = maxLinearVelocity;
    motion->m_motionState.m_deactivationCounter = 20;
    motion->m_motionState.m_maxAngularVelocity = maxAngularVelocity;
}

hkRigidBody::hkRigidBody(const hkRigidBodyCinfo& info) : hkEntity(info.m_shape) {
    m_material.setResponseType(info.m_collisionResponse);
    m_processContactCallbackDelay = info.m_processContactCallbackDelay;
    m_collidable.m_broadPhaseHandle.m_collisionFilterInfo = info.m_collisionFilterInfo;

    if (info.m_motionType == hkMotion::MOTION_FIXED) {
        hkMotion* motion = new (&m_motion) hkFixedRigidMotion(info.m_position, info.m_rotation);
        motion->m_motionState.m_maxLinearVelocity = info.m_maxLinearVelocity;
        motion->m_motionState.m_maxAngularVelocity = info.m_maxAngularVelocity;
        motion->setDeactivationClass(1);
        m_collidable.m_motion = (hkMotion*)&m_motion.m_motionState;
        if (info.m_allowedPenetrationDepth <= 0.0f) {
            m_collidable.m_allowedPenetrationDepth = 3.40282e38f;
        } else {
            m_collidable.m_allowedPenetrationDepth = info.m_allowedPenetrationDepth;
        }
    } else {
        createDynamicRigidMotion(info.m_motionType, info.m_position, info.m_rotation, info.m_mass, info.m_maxLinearVelocity,
                    info.m_maxAngularVelocity, info.m_inertiaTensor, info.m_centerOfMass, &m_motion);
        m_motion.setDeactivationClass(info.m_solverDeactivation);
        setLinearVelocity(info.m_linearVelocity);
        setAngularVelocity(info.m_angularVelocity);
        m_collidable.m_motion = (hkMotion*)&m_motion.m_motionState;
        setDeactivator(info.m_rigidBodyDeactivatorType);
        m_collidable.m_allowedPenetrationDepth = info.m_allowedPenetrationDepth;
    }

    hkReal linearDamping, angularDamping;
    linearDamping = info.m_linearDamping;
    angularDamping = info.m_angularDamping;
    m_motion.m_motionState.m_linearDamping = linearDamping;
    m_motion.m_motionState.m_angularDamping = angularDamping;

    if (m_collidable.m_shape) {
        hkVector4 extents;
        updateCachedShapeInfo(m_collidable.m_shape, &extents);
        if (m_collidable.m_allowedPenetrationDepth <= 0.0f) {
            fn_802E2378((char*)&m_collidable, m_collidable.m_allowedPenetrationDepth, &extents.x);
        }
    }

    if (info.m_qualityType != hkRigidBodyCinfo::QUALITY_INVALID) {
        m_collidable.m_broadPhaseHandle.m_objectQualityType = info.m_qualityType;
    } else if (isFixed()) {
        m_collidable.m_broadPhaseHandle.m_objectQualityType = 1;
    } else if (info.m_motionType == hkMotion::MOTION_KEYFRAMED) {
        m_collidable.m_broadPhaseHandle.m_objectQualityType = 2;
    } else {
        m_collidable.m_broadPhaseHandle.m_objectQualityType = 3;
    }

    m_autoRemoveLevel = info.m_autoRemoveLevel;
    hkReal friction, restitution;
    friction = info.m_friction;
    restitution = info.m_restitution;
    m_material.m_friction = friction;
    m_material.m_restitution = restitution;
}

// MATCH-ONLY: the original keeps each field's load next to its store (scheduling off reproduces that, apart from
// the prologue order).
#pragma push
#pragma scheduling off
void hkRigidBody::getCinfo(hkRigidBodyCinfo& info) const {
    info.m_autoRemoveLevel = m_autoRemoveLevel;
    info.m_processContactCallbackDelay = m_processContactCallbackDelay;
    info.m_rigidBodyDeactivatorType = m_deactivator
                                          ? ((hkRigidBodyDeactivator*)m_deactivator)->getRigidBodyDeactivatorType()
                                          : hkRigidBodyDeactivator::DEACTIVATOR_NEVER;
    info.m_friction = m_material.m_friction;
    info.m_collisionResponse = (hkMaterial::ResponseType)m_material.m_responseType;
    info.m_restitution = m_material.m_restitution;
    info.m_linearDamping = m_motion.m_motionState.m_linearDamping;
    info.m_angularDamping = m_motion.m_motionState.m_angularDamping;
    info.m_linearVelocity = m_motion.m_linearVelocity;
    info.m_angularVelocity = m_motion.m_angularVelocity;
    info.m_mass = m_motion.getMass();
    m_motion.getInertiaLocal(info.m_inertiaTensor);
    info.m_motionType = (hkMotion::MotionType)m_motion.m_type;
    info.m_solverDeactivation = (hkRigidBodyCinfo::SolverDeactivation)m_motion.m_motionState.m_deactivationClass;
    info.m_maxLinearVelocity = m_motion.m_motionState.m_maxLinearVelocity;
    info.m_maxAngularVelocity = m_motion.m_motionState.m_maxAngularVelocity;
    info.m_position = m_motion.m_motionState.m_transform.m_translation;
    info.m_rotation = m_motion.m_motionState.m_sweptTransform.m_rotation1;
    info.m_centerOfMass = m_motion.m_motionState.m_sweptTransform.m_centerOfMassLocal;
    info.m_shape = m_collidable.m_shape;
    info.m_collisionFilterInfo = m_collidable.m_broadPhaseHandle.m_collisionFilterInfo;
    info.m_allowedPenetrationDepth = m_collidable.m_allowedPenetrationDepth;
    info.m_qualityType = (hkRigidBodyCinfo::QualityType)m_collidable.m_broadPhaseHandle.m_objectQualityType;
}
#pragma pop

bool hkRigidBody::setShape(hkShape* shape) {
    if (m_world && m_world->areCriticalOperationsLocked()) {
        struct {
            u8 m_type;            // 0x00 HYPOTHESIS: the record of the queued shape change
            hkRigidBody* m_body;  // 0x04
            hkShape* m_shape;     // 0x08
        } op;
        op.m_type = hkWorldOperation::WORLD_OBJECT_SET_SHAPE;
        op.m_body = this;
        op.m_shape = shape;
        m_world->queueOperation(&op);
        return false;
    }

    if (m_world) {
        m_world->lockCriticalOperations();
        hkWorldOperationUtil::removeEntityBP(m_world, this);
    }

    hkBool hadShape = m_collidable.m_shape != NULL;
    if (m_collidable.m_shape) {
        m_collidable.m_shape->removeReference();
    }
    m_collidable.m_shape = shape;
    shape->addReference();

    hkVector4 extents;
    updateCachedShapeInfo(shape, &extents);
    // an estimated penetration depth is estimated again for the new shape
    if (hadShape && m_collidable.m_allowedPenetrationDepth != 3.40282e38f) {
        m_collidable.m_allowedPenetrationDepth = -1.0f;
    }
    if (m_collidable.m_allowedPenetrationDepth <= 0.0f) {
        fn_802E2378((char*)&m_collidable, m_collidable.m_allowedPenetrationDepth, &extents.x);
    }

    if (m_world) {
        hkWorldCallbackUtil::fireEntityShapeSet(m_world, this);
    }
    hkEntityCallbackUtil::fireEntityShapeSet(this);

    if (m_world) {
        hkWorldOperationUtil::addEntityBP(m_world, this);
        m_world->unlockAndAttemptToExecutePendingOperations();
    }
    return true;
}

// TODO: cleanup (shape from jj-link/brawl fn_802E3284)
// fn_802E3284: body from https://github.com/jj-link/brawl src/havok/fn_802E3284.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkRigidBody.h>
#include <havok/hkSpatialRigidBodyDeactivator.h>
namespace jj_fn_802E3284 {
class Class_80487F38 {
public:
    virtual void fn_802E3814(int);
    virtual void fn_80080A64();
    virtual void fn_802E3958();
    virtual void fn_802E3A90();
    virtual int fn_802E3950();
    unsigned short size;
    short refs;
};
class SoHeapAllocator {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual Class_80487F38* alloc(int, int);
};
struct Owner {
    char pad[0x68];
    Class_80487F38* object;
};
extern "C" char lbl_80532FF0[16];
extern "C" void fn_802E3284(Owner* self, int kind) {
    if (self->object == 0 || kind != self->object->fn_802E3950()) {
        switch (kind) {
        case 1:
            ((hkRigidBody*)(self))->setDeactivator((hkEntityDeactivator*)((Class_80487F38*)lbl_80532FF0));
            break;
        case 2:
            Class_80487F38* p = (Class_80487F38*)new hkSpatialRigidBodyDeactivator();
            ((hkRigidBody*)(self))->setDeactivator((hkEntityDeactivator*)(p));
            if (p->size != 0) {
                if (--p->refs == 0 && p != 0)
                    p->fn_802E3814(1);
            }
            break;
        }
    }
}
} // namespace jj_fn_802E3284
