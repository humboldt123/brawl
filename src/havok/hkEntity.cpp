// Havok translation unit hkEntity.o (main.dol 0x802E0E40-0x802E1FCC).
// Functions in address order (method names from the Havok TU map):
//   0x802E0E40    52  finishLoadedObjecthkEntity   [map: hkEntity__finishLoadedObjecthkEntity]
//   0x802E0E74    20  cleanupLoadedObjecthkEntity   [map: hkEntity__cleanupLoadedObjecthkEntity]
//   0x802E0E88    72  getVtablehkEntity   [map: hkEntity__getVtablehkEntity]
//   0x802E0ED0   228  __ct   [map: hkEntity____ct]
//   0x802E0FB4   284  __dt   [map: hkWorldObject____dt]
//   0x802E10D0    80  __dt   [map: hkMotion____dt]
//   0x802E1120   100  __dt   [map: hkMaxSizeMotion____dt]
//   0x802E1184   440  __ct   [map: hkEntity____ct1]
//   0x802E133C    80  __dt   [map: hkSphereMotion____dt]
//   0x802E138C    80  __dt   [map: hkBoxMotion____dt]
//   0x802E13DC   380  calcStatistics   [map: hkEntity__calcStatistics]
//   0x802E1558   700  __dt   [map: hkEntity____dt]
//   0x802E1814   156  setDeactivator   [map: hkEntity__setDeactivator]
//   0x802E18B0   192  addEntityListener   [map: hkEntity__addEntityListener]
//   0x802E1970   192  addCollisionListener   [map: hkEntity__addCollisionListener]
//   0x802E1A30    36  isActive   [map: hkEntity__isActive]
//   0x802E1A54   128  activate   [map: hkEntity__activate]
//   0x802E1AD4    60  deactivate   [map: hkEntity__deactivate]
//   0x802E1B10   816  deallocateInternalArrays   [map: hkEntity__deallocateInternalArrays]
//   0x802E1E40    16  getNumConstraints   [map: hkEntity__getNumConstraints]
//   0x802E1E50    48  getConstraint   [map: hkEntity__getConstraint]
//   0x802E1E80     8  getMotionState   [map: hkEntity__getMotionState]
//   0x802E1E88    80  __dt   [map: hkStabilizedSphereMotion____dt]
//   0x802E1ED8    80  __dt   [map: hkStabilizedBoxMotion____dt]
//   0x802E1F28    80  __dt   [map: hkThinBoxMotion____dt]
//   0x802E1F78    84  __sinit_\hkEntity_cpp   [map: hkEntitycpp____sinit_]

#include <new>
#include <havok/hkEntity.h>
#include <havok/hkRegistry.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkSphereMotion.h>
#include <havok/hkStabilizedSphereMotion.h>
#include <havok/hkBoxMotion.h>
#include <havok/hkStabilizedBoxMotion.h>
#include <havok/hkFixedRigidMotion.h>
#include <havok/hkThinBoxMotion.h>
#include <havok/hkEntityCallbackUtil.h>
#include <havok/hkEntityDeactivator.h>


void hkEntity::finishLoadedObjecthkEntity(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    new (p) hkEntity(flag);
}

void hkEntity::cleanupLoadedObjecthkEntity(void* p) {
    ((hkEntity*)p)->~hkEntity();
}

#pragma push
#pragma auto_inline off
const void* hkEntity::getVtablehkEntity() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    hkVector4 buf[31]; // 0x1F0 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkEntity(flag);
    return *(const void**)buf;
}
#pragma pop

// The motion is loaded as raw bytes: rebuild the object of its type in place so it gets its vtable back.
// Dynamic (type 1) motions are not rebuilt.
hkEntity::hkEntity(hkFinishLoadedObjectFlag flag) : hkWorldObject(flag), m_motion(flag) {
    if (flag.m_finishing) {
        switch (m_motion.m_type) {
        case hkMotion::MOTION_SPHERE_INERTIA:
            new (&m_motion) hkSphereMotion(flag);
            break;
        case hkMotion::MOTION_STABILIZED_SPHERE_INERTIA:
            new (&m_motion) hkStabilizedSphereMotion(flag);
            break;
        case hkMotion::MOTION_BOX_INERTIA:
            new (&m_motion) hkBoxMotion(flag);
            break;
        case hkMotion::MOTION_STABILIZED_BOX_INERTIA:
            new (&m_motion) hkStabilizedBoxMotion(flag);
            break;
        case hkMotion::MOTION_KEYFRAMED:
            new (&m_motion) hkKeyframedRigidMotion(flag);
            break;
        case hkMotion::MOTION_FIXED:
            new (&m_motion) hkFixedRigidMotion(flag);
            break;
        case hkMotion::MOTION_THIN_BOX_INERTIA:
            new (&m_motion) hkThinBoxMotion(flag);
            break;
        }
    }
}

hkEntity::~hkEntity() {
    hkEntityCallbackUtil::fireEntityDeleted(this);
    if (m_deactivator) {
        m_deactivator->removeReference();
    }
}

// Replaces the deactivator; reference counts are kept on both objects.
void hkEntity::setDeactivator(hkEntityDeactivator* deactivator) {
    if (deactivator != 0) {
        ((hkReferencedObject*)deactivator)->addReference();
    }
    if (m_deactivator != 0) {
        ((hkReferencedObject*)m_deactivator)->removeReference();
    }
    m_deactivator = deactivator;
}

// Appends the listener to the first empty slot, or to the end of the array.
void hkEntity::addEntityListener(hkEntityListener* listener) {
    // A free (null) slot left by a removal is reused before the array grows.
    int idx = m_entityListeners.indexOf(NULL);
    if (idx >= 0) {
        m_entityListeners[idx] = listener;
    } else {
        m_entityListeners.pushBack(listener);
    }
}

void hkEntity::addCollisionListener(hkCollisionListener* listener) {
    // A free (null) slot left by a removal is reused before the array grows.
    int idx = m_collisionListeners.indexOf(NULL);
    if (idx >= 0) {
        m_collisionListeners[idx] = listener;
    } else {
        m_collisionListeners.pushBack(listener);
    }
}

// HYPOTHESIS: the activity bits are returned in the top byte.
u32 hkEntity::isActive() const {
    return (u32)getActivationState() << 24;
}

void hkEntity::activate() {
    bool wantActivate = false;
    bool notFixed = false;
    if ((s8)(u8)getActivationState() == 0) {
        notFixed = !(getMotion()->m_type == hkMotion::MOTION_FIXED);
    }
    if (notFixed && m_world != 0) {
        wantActivate = true;
    }
    if (wantActivate) {
        hkWorldOperationUtil::markIslandActive(m_world, m_simulationIsland);
    }
}

void hkEntity::deactivate() {
    if ((s8)(u8)getActivationState() != 0) {
        hkWorldOperationUtil::markIslandInactive(m_world, m_simulationIsland);
    }
}

int hkEntity::getNumConstraints() const {
    return m_constraintsMaster.m_size + m_constraintsSlave.m_size;
}

hkConstraintInstance* hkEntity::getConstraint(int index) const {
    int numMaster = m_constraintsMaster.m_size;
    if (index < numMaster) {
        return *(hkConstraintInstance**)((u8*)m_constraintsMaster.m_data + index * 0x24);
    }
    return ((hkConstraintInstance**)m_constraintsSlave.m_data)[index - numMaster];
}

hkMotionState* hkEntity::getMotionState() {
    return 0;
}

static hkStaticTypeInfo hkEntityTypeInfo("hkEntity", hkEntity::finishLoadedObjecthkEntity, hkEntity::cleanupLoadedObjecthkEntity,
                                      hkEntity::getVtablehkEntity());

// fn_802E13DC: body from https://github.com/jj-link/brawl src/havok/fn_802E13DC.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
#include <havok/hkWorldObject.h>
namespace jj_fn_802E13DC {
extern "C" char lbl_804129D0[];

struct Entity {
    char pad0[0x68];
    void* f68;
    char pad6C[0x1A0 - 0x6C];
    void* f1A0;
    char pad1A4[0x1B4 - 0x1A4];
    void* f1B4;
    u32 f1B8;
    u32 f1BC;
    void* f1C0;
    u32 f1C4;
    u32 f1C8;
    void* f1CC;
    u32 f1D0;
    u32 f1D4;
};

class Visitor {
public:
    virtual void slot0();
    virtual void slot1(const char* name, int id, void* data);
    virtual void slot2(const char* name, int size, void* value, u32 length, u32 count);
    virtual void slot3(const char* name, int size, void* value);
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
};

extern "C" {


// TODO: cleanup (shape from jj-link/brawl fn_802E13DC)
void fn_802E13DC(Entity* arg0, Visitor* arg1) {
    arg1->slot1(lbl_804129D0, 2, arg0);
    ((const hkWorldObject*)(arg0))->hkWorldObject::calcStatistics((hkStatisticsCollector*)(arg1));
    arg1->slot3(lbl_804129D0 + 7, 4, arg0->f1A0);
    arg1->slot3(lbl_804129D0 + 0x13, 4, arg0->f68);

    u32 flags1 = arg0->f1BC;
    const char* name1 = lbl_804129D0 + 0x1F;
    if (!(flags1 & 0x80000000)) {
        arg1->slot2(name1, 4, arg0->f1B4, arg0->f1B8 * 4, flags1 * 4);
    }

    u32 flags2 = arg0->f1C8;
    const char* name2 = lbl_804129D0 + 0x2F;
    if (!(flags2 & 0x80000000)) {
        arg1->slot2(name2, 4, arg0->f1C0, arg0->f1C4 * 4, flags2 * 4);
    }

    u32 flags3 = arg0->f1D4;
    const char* name3 = lbl_804129D0 + 0x3C;
    if (!(flags3 & 0x80000000)) {
        arg1->slot2(name3, 4, arg0->f1CC, arg0->f1D0 * 4, flags3 * 4);
    }

    arg1->slot6();
}

}
} // namespace jj_fn_802E13DC
