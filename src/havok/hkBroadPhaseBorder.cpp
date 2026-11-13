// Havok translation unit hkBroadPhaseBorder.o (main.dol 0x802E911C-0x802E9890).
// Functions in address order (method names from the Havok TU map):
//   0x802E911C  1224  __ct   [map: hkBroadPhaseBorder____ct]
//   0x802E95E4   172  __dt   [map: hkBroadPhaseBorder____dt]
//   0x802E9690    60  collidableAddedCallback   [map: hkBroadPhaseBorder__collidableAddedCallback]
//   0x802E96CC     4  collidableRemovedCallback   [map: hkBroadPhaseBorder__collidableRemovedCallback]
//   0x802E96D0    92  maxPositionExceededCallback   [map: hkBroadPhaseBorder__maxPositionExceededCallback]
//   0x802E972C   128  worldDeletedCallback   [map: hkBroadPhaseBorder__worldDeletedCallback]
//   0x802E97AC   188  deactivate   [map: hkBroadPhaseBorder__deactivate]
//   0x802E9868     8  @8@worldDeletedCallback   [map: hkBroadPhaseBorder___8_worldDeletedCallback]
//   0x802E9870     8  @8@   [map: hkBroadPhaseBorder___8_]
//   0x802E9878     8  @12@   [map: hkBroadPhaseBorder___12_]
//   0x802E9880     8  @12@collidableRemovedCallback   [map: hkBroadPhaseBorder___12_collidableRemovedCallback]
//   0x802E9888     8  @12@collidableAddedCallback   [map: hkBroadPhaseBorder___12_collidableAddedCallback]

#include <havok/hkBroadPhaseBorder.h>
#include <havok/hkCollidable.h>
#include <havok/hkRigidBody.h>
#include <havok/hkAabbPhantom.h>

// One phantom on the box in `aabb`: a border collidable that reports overlaps to this listener.
#define HK_BORDER_PHANTOM(INDEX, MIN_X, MIN_Y, MIN_Z, MAX_X, MAX_Y, MAX_Z)                    {                                                                                               aabb.m_min.set(MIN_X, MIN_Y, MIN_Z, 0.0f);                                                  aabb.m_max.set(MAX_X, MAX_Y, MAX_Z, 0.0f);                                                  hkAabbPhantom* phantom = new hkAabbPhantom(aabb);                                           phantom->m_collidable.m_broadPhaseHandle.m_type = hkWorldObject::BROAD_PHASE_BORDER;         phantom->addPhantomOverlapListener(this);                                                   m_phantoms[INDEX] = phantom;                                                                world->addPhantom(phantom);                                                             }

hkBroadPhaseBorder::hkBroadPhaseBorder(hkWorld* world, Type type) {
    m_world = world;
    m_type = type;
    addReference();
    m_world->addWorldDeletionListener(this);

    hkVector4 worldMin = *(const hkVector4*)m_world->m_broadPhaseExtents[0];
    hkVector4 worldMax = *(const hkVector4*)m_world->m_broadPhaseExtents[1];
    hkAabb aabb; // one box reused for all six phantoms
    HK_BORDER_PHANTOM(0, worldMax.x, worldMin.y, worldMin.z, worldMax.x, worldMax.y, worldMax.z)
    HK_BORDER_PHANTOM(1, worldMin.x, worldMin.y, worldMin.z, worldMin.x, worldMax.y, worldMax.z)
    HK_BORDER_PHANTOM(2, worldMin.x, worldMax.y, worldMin.z, worldMax.x, worldMax.y, worldMax.z)
    HK_BORDER_PHANTOM(3, worldMin.x, worldMin.y, worldMin.z, worldMax.x, worldMin.y, worldMax.z)
    HK_BORDER_PHANTOM(4, worldMin.x, worldMin.y, worldMax.z, worldMax.x, worldMax.y, worldMax.z)
    HK_BORDER_PHANTOM(5, worldMin.x, worldMin.y, worldMin.z, worldMax.x, worldMax.y, worldMin.z)
}

hkBroadPhaseBorder::~hkBroadPhaseBorder() {
    for (int i = 0; i < 6; i++) {
        if (m_phantoms[i] != 0) {
            m_phantoms[i]->removeReference();
            m_phantoms[i] = 0;
        }
    }
}

// Only overlaps with entities count: the owner of an entity collidable is found through its owner offset.
void hkBroadPhaseBorder::collidableAddedCallback(hkCollidableAddedEvent* event) {
    hkCollidable* collidable = event->m_collidable;
    hkEntity* entity;
    if (collidable->m_broadPhaseHandle.m_type == 1) {
        entity = (hkEntity*)((u8*)collidable + collidable->m_ownerOffset);
    } else {
        entity = 0;
    }
    if (entity != 0) {
        maxPositionExceededCallback(entity);
    }
}

void hkBroadPhaseBorder::collidableRemovedCallback(hkCollidableRemovedEvent* event) {}

// BYTES: the case order (assert, remove, fix) follows the target's block order.
void hkBroadPhaseBorder::maxPositionExceededCallback(hkEntity* entity) {
    switch (m_type) {
    case BROADPHASE_BORDER_ASSERT:
        ((hkRigidBody*)entity)->setMotionType(7, 1, 0); // HYPOTHESIS: 7 = fixed motion
        break;
    case BROADPHASE_BORDER_REMOVE_ENTITY:
        m_world->removeEntity(entity);
        break;
    case BROADPHASE_BORDER_FIX_ENTITY:
        ((hkRigidBody*)entity)->setMotionType(7, 1, 0);
        break;
    }
}

void hkBroadPhaseBorder::worldDeletedCallback(hkWorld* world) {
    m_world->removeWorldDeletionListener(this);
    removeReference();
}

void hkBroadPhaseBorder::deactivate() {
    if (m_world != 0) {
        m_world->removePhantomBatch(m_phantoms, 6);
        for (int i = 0; i < 6; i++) {
            m_phantoms[i]->removePhantomOverlapListener(this);
            m_phantoms[i]->removeReference();
            m_phantoms[i] = 0;
        }
        worldDeletedCallback(m_world);
        m_world = 0;
    }
}
