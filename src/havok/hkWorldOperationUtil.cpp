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
#include <havok/hkAction.h>
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
    phantom->calcAabb(aabb);

    // world +0x54 is the broadphase
    at<hkBroadPhase*>(world, 0x54)->addObject(&phantom->m_collidable.m_broadPhaseHandle, aabb, newPairs);

    if (newPairs.m_size != 0) {
        void* filter = at<void*>(world, 0x6C);
        if (filter != NULL) {
            filter = reinterpret_cast<char*>(filter) + 8; // the collision filter interface sits 8 bytes in
        }
        at<hkTypedBroadPhaseDispatcher*>(world, 0x58)
            ->addPairs(newPairs.begin(), newPairs.getSize(), (const hkCollidableCollidableFilter*)filter);
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
            ->addPairs(newPairs.begin(), newPairs.getSize(), (const hkCollidableCollidableFilter*)filter);
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

void hkWorldOperationUtil::removeEntitySI(hkWorld* world, hkEntity* entity) {
    entity->m_world = NULL;
    hkSimulationIsland* island = entity->m_simulationIsland;
    island->internalRemoveEntity(entity);
    // an emptied dynamic island goes away
    if ((!(bool)island->isFixed() && island->m_entities.getSize() == 0) && (bool)world->m_wantSimulationIslands) {
        removeIsland(world, island);
        delete island;
    }
}

// Actions of the entity that sit on the fixed island are collected (with a reference) to move them to the
// entity's new dynamic island.
void hkWorldOperationUtil::removeAttachedActionsFromFixedIsland(hkWorld* world, hkEntity* entity,
                                                                hkArray<hkAction*>& actionsToBeMoved) {
    for (int i = 0; i < entity->m_actions.getSize(); i++) {
        hkAction* action = entity->m_actions[i];
        if (action->m_island == world->m_fixedIsland) {
            actionsToBeMoved.pushBack(action);
            action->addReference();
            world->m_fixedIsland->removeAction(action);
            world->m_fixedIsland->m_flags.m_stateB = 1; // the fixed island's action list needs a cleanup
            hkSimulationIsland* island = world->m_fixedIsland;
            if (island->m_dirtyListIndex == 0xFFFF) {
                island->m_dirtyListIndex = world->m_unk40.m_size;
                ((hkArray<hkSimulationIsland*>&)world->m_unk40).pushBack(island);
            }
        }
    }
}

// Actions whose other entities are all fixed follow the entity to the fixed island: they are collected (with a
// reference) and taken off the entity's dynamic island.
void hkWorldOperationUtil::removeAttachedActionsFromDynamicIsland(hkWorld* world, hkEntity* entity,
                                                                  hkArray<hkAction*>& actionsToBeMoved) {
    hkSimulationIsland* island = entity->m_simulationIsland;
    hkInplaceArray<hkEntity*, 16> actionEntities;
    for (int i = 0; i < entity->m_actions.getSize(); i++) {
        hkAction* action = entity->m_actions[i];
        action->getEntities(actionEntities);

        hkBool moveAction = true;
        for (int j = 0; j < actionEntities.getSize(); j++) {
            if (!(bool)actionEntities[j]->isFixed() && actionEntities[j] != entity) {
                moveAction = false;
                break;
            }
        }

        if (moveAction) {
            actionsToBeMoved.pushBack(action);
            action->addReference();
            island->removeAction(action);
            island->m_flags.m_stateB = 1; // the island's action list needs a cleanup
            if (island->m_dirtyListIndex == 0xFFFF) {
                island->m_dirtyListIndex = world->m_unk40.m_size;
                ((hkArray<hkSimulationIsland*>&)world->m_unk40).pushBack(island);
            }
        }
    }
}

// Processes the islands queued on the world's dirty list (last first; callbacks may add more): removed actions
// are compacted out of the action lists, and islands whose activity changed are moved between the active and
// inactive island arrays.
void hkWorldOperationUtil::cleanupDirtyIslands(hkWorld* world) {
    hkArray<hkSimulationIsland*>& dirtyIslands = (hkArray<hkSimulationIsland*>&)world->m_unk40;
    while (dirtyIslands.getSize()) {
        hkSimulationIsland* island = dirtyIslands[dirtyIslands.getSize() - 1];
        dirtyIslands.popBack();
        if (island) {
            island->m_dirtyListIndex = 0xFFFF;

            if (island->m_flags.m_stateB) {
                hkArray<hkAction*>& actions = island->m_actions;
                int freeSlot = -1;
                int a = 0;
                for (; a < actions.getSize(); a++) {
                    if (actions[a] == NULL) {
                        freeSlot = a;
                        a++;
                        break;
                    }
                }
                for (; a < actions.getSize(); a++) {
                    if (actions[a] != NULL) {
                        actions[freeSlot++] = actions[a];
                    }
                }
                if (freeSlot != -1) {
                    actions.setSize(freeSlot);
                }
                island->m_flags.m_stateB = 0;
            }

            if (island->m_flags27.m_active != island->m_flags.m_stateD) {
                if (island->m_flags27.m_active) {
                    internalActivateIsland(world, island);
                } else {
                    internalDeactivateIsland(world, island);
                }
            }
        }
    }
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

// BYTES: not an inline candidate (removeEntitySI calls it out of line).
#pragma push
#pragma auto_inline off
void hkWorldOperationUtil::removeIsland(hkWorld* world, hkSimulationIsland* island) {
    if ((island->flags26() & 3) == 0) {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk34, island);
    } else {
        removeIslandFromArray(*(hkArray<hkSimulationIsland*>*)&world->m_unk28, island);
    }
    removeIslandFromDirtyList(world, island);
}
#pragma pop

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

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// 12-byte swap (unnamed in the map)
// TODO: cleanup (shape from jj-link/brawl fn_802FB210)
extern "C" void fn_802FB210(int* a, int* b) {
    int t;
    t = a[0]; a[0] = b[0]; b[0] = t;
    t = a[1]; a[1] = b[1]; b[1] = t;
    t = a[2]; a[2] = b[2]; b[2] = t;
}

// TODO: cleanup (shape from jj-link/brawl fn_802F83E8)
// fn_802F83E8: body from https://github.com/jj-link/brawl src/havok/fn_802F83E8.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkWorld.h>
namespace jj_fn_802F83E8 {
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void* slot4(u32 size, s32 align);
};

struct Node {
    u8 pad0[4];
    s16 size;
    u8 pad6[0x20 - 6];
    s16 index;
    u8 pad22[4];
    u8 pad26a : 6;
    u8 b : 2;
    u8 a : 2;
    u8 pad27a : 6;
};

struct Owner {
    u8 pad0[0x20];
    Node* node;
    u8 pad24[4];
    Node** data1;
    s32 size1;
    u32 cap1;
    Node** data2;
    s32 size2;
    u32 cap2;
    u8 pad40[0xb4 - 0x40];
    signed char flag;
};

struct Flag {
    signed char v;
};

struct Target {
    u8 pad0[8];
    Owner* owner;
    u8 padc[0xa8 - 0xc];
    u8 kind;
    Flag isKind7() {
        Flag f;
        f.v = kind == 7;
        return f;
    }
};


extern "C" void fn_802F83E8(Owner* owner, Target* target, int mode) {
    target->owner = owner;
    if (target->isKind7().v) {
        ((hkSimulationIsland*)(owner->node))->internalAddEntity((hkEntity*)(target));
        return;
    }
    if (owner->flag) {
        Node* n = (Node*)new hkSimulationIsland((hkWorld*)owner);
        if (mode == 1) {
            n->a = 1;
            n->b = 1;
            n->index = owner->size1;
            if (owner->size1 == (s32)(owner->cap1 & 0x3FFFFFFF)) {
                hkArrayUtil::_reserveMore((hkArrayBase*)(&owner->data1), 4);
            }
            owner->data1[owner->size1++] = n;
        } else {
            n->a = 0;
            n->b = 0;
            n->index = owner->size2;
            if (owner->size2 == (s32)(owner->cap2 & 0x3FFFFFFF)) {
                hkArrayUtil::_reserveMore((hkArrayBase*)(&owner->data2), 4);
            }
            owner->data2[owner->size2++] = n;
        }
        ((hkSimulationIsland*)(n))->internalAddEntity((hkEntity*)(target));
        return;
    }
    if (mode == 1) {
        ((hkSimulationIsland*)(owner->data1[0]))->internalAddEntity((hkEntity*)(target));
    }
}
} // namespace jj_fn_802F83E8
