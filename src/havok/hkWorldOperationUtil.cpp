// Havok translation unit hkWorldOperationUtil.o (main.dol 0x802F7E24-0x802FB244).
// Work in progress. Functions in address order (method names from the Havok TU map):
//   0x802F7E24  updateEntityBP, addEntityBP, addPhantomBP, addEntitySI, removeEntityBP, removePhantomBP,
//   removeEntitySI, removeAttachedActionsFromFixedIsland, removeAttachedActionsFromDynamicIsland,
//   addActionsToEntitysIsland, removeIsland, addConstraintToCriticalLockedIsland,
//   removeConstraintFromCriticalLockedIsland, addConstraintImmediately, removeConstraintImmediately,
//   splitSimulationIslands, splitSimulationIslands1, mergeIslands, internalMergeTwoIslands,
//   setRigidBodyMotionType, removeAttachedConstraints, ..., removeIslandFromDirtyList, ...

#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkWorldConstraintUtil.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>
#include <havok/hkLocalArray.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkPhantom.h>
#include <havok/hkSimulation.h>
#include <havok/hkShape.h>
#include <havok/hkTypedBroadPhaseDispatcher.h>

// HYPOTHESIS: the top two bits of the byte at island+0x26/+0x27 hold the activity state, written as a 2-bit field.
// Local view only; hkSimulationIsland.h does not declare these bytes yet.
struct hkIslandActivityByte {
    u8 activity : 2;
    u8 rest : 6;
};

// world members nobody has laid out yet, and the broadphase pieces behind them (raw offsets from the asm)
template <typename T>
static inline T& at(void* p, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(p) + offset);
}


// puts the phantom in the broadphase, then lets the dispatcher react to every pair that showed up
void hkWorldOperationUtil::addPhantomBP(hkWorld* world, hkPhantom* phantom) {
    hkLocalArray<hkBroadPhaseHandlePair> newPairs(at<int>(world, 0x2E4)); // world's broadphase query size

    hkAabb aabb;
    phantom->calcAabb(&aabb);

    // world +0x54 is the broadphase
    at<hkBroadPhase*>(world, 0x54)->addObject(&phantom->m_collidable.m_broadPhaseHandle, aabb, newPairs);

    if (newPairs.m_size != 0) {
        void* filter = at<void*>(world, 0x6C);
        if (filter != NULL) {
            filter = reinterpret_cast<char*>(filter) + 8; // the collision filter interface sits 8 bytes in
        }
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)
            ->addPairs(newPairs.begin(), newPairs.getSize(), (const hkCollisionFilterInterface0*)filter);
    }
}

void hkWorldOperationUtil::removePhantomBP(hkWorld* world, hkPhantom* phantom) {
    hkLocalArray<hkBroadPhaseHandlePair> removedPairs(at<int>(world, 0x2E4));

    // the broadphase takes the object out and reports the pairs that went with it
    at<hkBroadPhase*>(world, 0x54)->removeObject(&phantom->m_collidable.m_broadPhaseHandle, removedPairs);

    if (removedPairs.m_size != 0) {
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)->removePairs(&removedPairs[0], removedPairs.m_size);
    }
}

// the entity version of addPhantomBP: same broadphase dance, but the box comes from the entity's shape,
// pushed out a bit by half of the world's collision tolerance
void hkWorldOperationUtil::addEntityBP(hkWorld* world, hkEntity* entity) {
    if (entity->m_collidable.m_shape == NULL) {
        return;
    }

    hkLocalArray<hkBroadPhaseHandlePair> newPairs(at<int>(world, 0x2E4));

    // HYPOTHESIS: world +0x68 holds the collision settings and +8 in there is the tolerance; the collidable's
    // motion field points at the entity's transform
    hkAabb aabb;
    entity->m_collidable.m_shape->getAabb(*reinterpret_cast<const hkTransform*>(entity->m_collidable.m_motion),
                                          0.5f * at<float>(at<void*>(world, 0x68), 8), aabb);

    at<hkBroadPhase*>(world, 0x54)->addObject(&entity->m_collidable.m_broadPhaseHandle, aabb, newPairs);

    if (newPairs.m_size > 0) {
        void* filter = at<void*>(world, 0x6C);
        if (filter != NULL) {
            filter = reinterpret_cast<char*>(filter) + 8;
        }
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)
            ->addPairs(newPairs.begin(), newPairs.getSize(), (const hkCollisionFilterInterface0*)filter);
    }
}

// takes the entity out of the broadphase. the simulation gets to throw away this entity's collision info first
void hkWorldOperationUtil::removeEntityBP(hkWorld* world, hkEntity* entity) {
    hkEntity* entities[1] = {entity};
    if (entity->m_collidable.m_shape == NULL) {
        return;
    }

    world->m_simulation->resetCollisionInformationForEntities(entities, 1, world, true);

    hkLocalArray<hkBroadPhaseHandlePair> removedPairs(at<int>(world, 0x2E4));
    at<hkBroadPhase*>(world, 0x54)->removeObject(&entity->m_collidable.m_broadPhaseHandle, removedPairs);

    if (removedPairs.m_size > 0) {
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)->removePairs(&removedPairs[0], removedPairs.m_size);
    }
}

void hkWorldOperationUtil::addConstraintToCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint) {
    hkWorldConstraintUtil::addConstraint(world, constraint);
    hkWorldCallbackUtil::fireConstraintAdded(world, constraint);
}

void hkWorldOperationUtil::removeConstraintFromCriticalLockedIsland(hkWorld* world, hkConstraintInstance* constraint) {
    if (hkWorldCallbackUtil::listenersAt<hkWorldCallbackUtil::ConstraintListener>(world, 0xFC).m_size != 0) {
        hkWorldCallbackUtil::fireConstraintRemoved(world, constraint);
    }
    hkWorldConstraintUtil::removeConstraint(constraint);
}

void hkWorldOperationUtil::splitSimulationIslands(hkWorld* world) {
    u8 flag = 1;
    splitSimulationIslands1(world, &flag);
}


// Islands whose state changes are queued in the world's dirty list (hkWorld::m_unk40, an hkArray of islands).
void hkWorldOperationUtil::markIslandInactive(hkWorld* world, hkSimulationIsland* island) {
    // Activity state (top two bits of the byte at 0x27) cleared; see hkEntity::getActivationState.
    ((hkIslandActivityByte*)((u8*)island + 0x27))->activity = 0;
    if (*(u16*)((u8*)island + 0x22) == 0xFFFF) {
        *(u16*)((u8*)island + 0x22) = (u16)world->m_unk40.m_size;
        if (world->m_unk40.m_size == (world->m_unk40.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK)) {
            hkArrayUtil::_reserveMore(&world->m_unk40, 4);
        }
        ((hkSimulationIsland**)world->m_unk40.m_data)[world->m_unk40.m_size++] = island;
    }
}

void hkWorldOperationUtil::markIslandActive(hkWorld* world, hkSimulationIsland* island) {
    u32 activity = 1;
    ((hkIslandActivityByte*)((u8*)island + 0x27))->activity = activity;
    *((u8*)island + 0x25) = 0;
    *((u8*)island + 0x24) = 0;
    if (*(u16*)((u8*)island + 0x22) == 0xFFFF) {
        *(u16*)((u8*)island + 0x22) = (u16)world->m_unk40.m_size;
        if (world->m_unk40.m_size == (world->m_unk40.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK)) {
            hkArrayUtil::_reserveMore(&world->m_unk40, 4);
        }
        ((hkSimulationIsland**)world->m_unk40.m_data)[world->m_unk40.m_size++] = island;
    }
}

// Removes an island from one of the world's island arrays by moving the last entry into its slot.
// The island's index (u16 at 0x20, not recovered in hkSimulationIsland.h) is kept up to date.
static void removeIslandFromArray(hkArray<hkSimulationIsland*>& islands, hkSimulationIsland* island) {
    islands[*(u16*)((u8*)island + 0x20)] = islands[islands.m_size - 1];
    hkSimulationIsland* moved = islands[*(u16*)((u8*)island + 0x20)];
    *(u16*)((u8*)moved + 0x20) = *(u16*)((u8*)island + 0x20);
    islands.m_size--;
}

#pragma dont_inline on
void hkWorldOperationUtil::removeIslandFromDirtyList(hkWorld* world, hkSimulationIsland* island) {
    // Dirty-list index of the island: u16 at 0x22 (not recovered in hkSimulationIsland.h).
    u16 index = *(u16*)((u8*)island + 0x22);
    if (index == 0xFFFF) {
        return;
    }
    ((int*)world->m_unk40.m_data)[index] = 0;
    *(u16*)((u8*)island + 0x22) = 0xFFFF;
}
#pragma dont_inline reset

void hkWorldOperationUtil::removeIsland(hkWorld* world, hkSimulationIsland* island) {
    if ((island->flags26() & 3) == 0) {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk34, island);
    } else {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk28, island);
    }
    removeIslandFromDirtyList(world, island);
}

hkConstraintInstance* hkWorldOperationUtil::addConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback) {
    world->m_lockCount++;
    hkWorldConstraintUtil::addConstraint(world, constraint);
    if (fireCallback) {
        hkWorldCallbackUtil::fireConstraintAdded(world, constraint);
    }
    world->unlockAndAttemptToExecutePendingOperations();
    return constraint;
}

void hkWorldOperationUtil::mergeIslands(hkWorld* world, hkEntity* entityA, hkEntity* entityB) {
    hkSimulationIsland* islandA = entityA->m_simulationIsland;
    hkSimulationIsland* islandB = entityB->m_simulationIsland;
    if (world->m_lockCount != 0) {
        hkWorldOperation::BiggestOperation operation;
        operation.m_type = 0x0C;
        operation.m_entityA = entityA;
        operation.m_entityB = entityB;
        world->queueOperation(&operation);
    } else {
        internalMergeTwoIslands(world, islandA, islandB);
    }
}

void hkWorldOperationUtil::removeConstraintImmediately(hkWorld* world, hkConstraintInstance* constraint, bool fireCallback) {
    // HYPOTHESIS: the constraint's word at 0x08 is the island whose activity state (bits 0xC0 of 0x26) is set.
    hkSimulationIsland* island = *(hkSimulationIsland**)((u8*)constraint + 0x08);
    ((hkIslandActivityByte*)((u8*)island + 0x26))->activity = 1;
    if (world->m_lockCount != 0) {
        if (fireCallback && hkWorldCallbackUtil::listenersAt<hkWorldCallbackUtil::ConstraintListener>(world, 0xFC).m_size != 0) {
            hkWorldCallbackUtil::fireConstraintRemoved(world, constraint);
        }
        hkWorldConstraintUtil::removeConstraint(constraint);
    } else {
        world->m_lockCount++;
        if (fireCallback && hkWorldCallbackUtil::listenersAt<hkWorldCallbackUtil::ConstraintListener>(world, 0xFC).m_size != 0) {
            hkWorldCallbackUtil::fireConstraintRemoved(world, constraint);
        }
        hkWorldConstraintUtil::removeConstraint(constraint);
        world->unlockAndAttemptToExecutePendingOperations();
    }
}
