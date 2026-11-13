// Havok translation unit hkWorldAgentUtil.o (main.dol 0x802F34F0-0x802F5D5C).
// Work in progress. Functions in address order (map names in brackets).
//   0x802F34F0   addAgent, removeAgent
//   0x802F377C   removeAgentAndItsToiEvents, getIslandFromAgentEntry, invalidateTim, warpTime
//   0x802F39A0   fireActionAdded, fireActionRemoved, fireEntityAdded, fireEntityRemoved, fireEntityShapeSet,
//   firePhantomAdded, firePhantomRemoved, fireConstraintAdded, fireConstraintRemoved (248 bytes each)

#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkWorldConstraintUtil.h>
#include <havok/hkEntity.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkWorldAgentUtil.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkWorld.h>
#include <havok/hkSimulation.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkWorldOperationUtil.h>

typedef hkArray<hkLinkedCollidable::CollisionEntry> CollisionEntryArray;

hkAgentNnEntry* hkWorldAgentUtil::addAgent(hkLinkedCollidable* collA, hkLinkedCollidable* collB,
                                           const hkProcessCollisionInput& input) {
    hkEntity* entityA = (hkEntity*)((char*)collA + collA->m_ownerOffset);
    hkEntity* entityB = (hkEntity*)((char*)collB + collB->m_ownerOffset);
    if (entityA->isFixed() == false && entityB->isFixed() == false && entityA->m_simulationIsland != entityB->m_simulationIsland) {
        hkWorldOperationUtil::mergeIslands(entityA->m_world, entityA, entityB);
    }
    // the agent goes to an island that is not the fixed one when there is one
    hkSimulationIsland* islandA = entityA->m_simulationIsland;
    hkSimulationIsland* islandB = entityB->m_simulationIsland;
    hkSimulationIsland* island;
    if (islandA->isFixed() == false) {
        island = islandA;
    } else if (islandB->isFixed() == false) {
        island = islandB;
    } else {
        island = islandA;
    }
    int agentType;
    int isFlipped;
    hkAgentNnMachine_GetAgentType(collA, collB, input, &agentType, &isFlipped);
    if (isFlipped) {
        hkLinkedCollidable* tmp = collA;
        collA = collB;
        collB = tmp;
    }
    hkContactMgr* mgr = input.m_dispatcher
                            ->m_contactMgrFactory[entityA->m_material.m_responseType][entityB->m_material.m_responseType]
                            ->createContactMgr(*collA, *collB, input);
    return hkAgentNnMachine_CreateAgent(island->m_agentTrack, collA, collB, agentType, input, mgr);
}

void hkWorldAgentUtil::removeAgent(hkAgentNnEntry* entry) {
    hkLinkedCollidable* collidableA = entry->m_collidable[0];
    hkLinkedCollidable* collidableB = entry->m_collidable[1];
    hkEntity* entityA = (hkEntity*)((char*)collidableA + collidableA->m_ownerOffset);
    hkEntity* entityB = (hkEntity*)((char*)collidableB + collidableB->m_ownerOffset);
    hkSimulationIsland* island;
    if (entityA->m_simulationIsland == entityB->m_simulationIsland) {
        island = entityA->m_simulationIsland;
        island->m_flags.m_stateA = 1;
    } else if (entityA->isFixed()) {
        island = entityB->m_simulationIsland;
    } else if (entityB->isFixed()) {
        island = entityA->m_simulationIsland;
    } else {
        island = getIslandFromAgentEntry(entry, entityA->m_simulationIsland, entityB->m_simulationIsland);
        entityA->m_simulationIsland->m_flags.m_stateA = 1;
        entityB->m_simulationIsland->m_flags.m_stateA = 1;
    }
    hkContactMgr* mgr = entry->m_contactMgr;
    hkAgentNnMachine_DestroyAgent(island->m_agentTrack, entry, island->m_world->m_collisionDispatcher);
    mgr->cleanup();
}

void hkWorldAgentUtil::removeAgentAndItsToiEvents(hkAgentNnEntry* entry) {
    hkLinkedCollidable* collidable = entry->m_collidable[0];
    hkEntity* entity = (hkEntity*)((char*)collidable + collidable->m_ownerOffset);
    entity->m_simulationIsland->m_world->m_simulation->removeCollisionInformationForAgent(entry);
    removeAgent(entry);
}

hkSimulationIsland* hkWorldAgentUtil::getIslandFromAgentEntry(hkAgentNnEntry* entry, hkSimulationIsland* candidateA,
                                                              hkSimulationIsland* candidateB) {
    hkBool searchA = candidateA->m_agentTrack.getNumSectors() <= candidateB->m_agentTrack.getNumSectors();
    hkSimulationIsland* island = searchA ? candidateA : candidateB;
    hkBool found = false;
    for (int i = 0; i < island->m_agentTrack.getNumSectors(); i++) {
        hkAgentNnSector* sector = island->m_agentTrack.getSector(i);
        if ((void*)sector <= (void*)entry && (void*)entry < (void*)((char*)sector + island->m_agentTrack.m_sectorSize)) {
            found = true;
            break;
        }
    }
    if (searchA ^ found) {
        return candidateB;
    }
    return candidateA;
}

void hkWorldAgentUtil::invalidateTim(hkEntity* entity, hkProcessCollisionInput& input) {
    hkLinkedCollidable* collidable = &entity->m_collidable;
    for (int i = 0; i < ((const CollisionEntryArray&)collidable->m_collisionEntries).getSize(); i++) {
        hkAgentNnMachine_InvalidateTimInAgent(
            ((const CollisionEntryArray&)collidable->m_collisionEntries)[i].m_agentEntry, input);
    }
}

void hkWorldAgentUtil::warpTime(hkSimulationIsland* island, hkReal oldTime, hkReal newTime,
                                hkProcessCollisionInput& input) {
    for (int i = 0; i < island->m_agentTrack.getNumSectors();) {
        hkAgentNnEntry* entry = (hkAgentNnEntry*)island->m_agentTrack.getSector(i);
        i++;
        hkAgentNnEntry* end = (i == island->m_agentTrack.getNumSectors())
                                  ? (hkAgentNnEntry*)((char*)entry + island->m_agentTrack.m_bytesUsedInLastSector)
                                  : (hkAgentNnEntry*)((char*)entry + island->m_agentTrack.m_sectorSize);
        for (; entry < end; entry = (hkAgentNnEntry*)((char*)entry + entry->m_size)) {
            hkAgentNnMachine_WarpTimeInAgent(entry, oldTime, newTime, input);
        }
    }
}

template <typename T>
void hkWorldCallbackUtil::removeNullListeners(hkArray<T*>& list) {
    for (int i = list.m_size - 1; i >= 0; i--) {
        if (list.begin()[i] == 0) {
            list.removeAtAndCopy(i);
        }
    }
}

void hkWorldCallbackUtil::fireActionAdded(hkWorld* world, hkAction* action) {
    hkArray<ActionListener*>& list = listenersAt<ActionListener>(world, 0xD8);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ActionListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->actionAdded(action);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireActionRemoved(hkWorld* world, hkAction* action) {
    hkArray<ActionListener*>& list = listenersAt<ActionListener>(world, 0xD8);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ActionListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->actionRemoved(action);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireEntityAdded(hkWorld* world, hkEntity* entity) {
    hkArray<EntityListener*>& list = listenersAt<EntityListener>(world, 0xE4);
    for (int i = list.m_size - 1; i >= 0; i--) {
        EntityListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->entityAdded(entity);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireEntityRemoved(hkWorld* world, hkEntity* entity) {
    hkArray<EntityListener*>& list = listenersAt<EntityListener>(world, 0xE4);
    for (int i = list.m_size - 1; i >= 0; i--) {
        EntityListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->entityRemoved(entity);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireEntityShapeSet(hkWorld* world, hkEntity* entity) {
    hkArray<EntityListener*>& list = listenersAt<EntityListener>(world, 0xE4);
    for (int i = list.m_size - 1; i >= 0; i--) {
        EntityListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->entityShapeSet(entity);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::firePhantomAdded(hkWorld* world, hkPhantom* phantom) {
    hkArray<PhantomListener*>& list = listenersAt<PhantomListener>(world, 0xF0);
    for (int i = list.m_size - 1; i >= 0; i--) {
        PhantomListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->phantomAdded(phantom);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::firePhantomRemoved(hkWorld* world, hkPhantom* phantom) {
    hkArray<PhantomListener*>& list = listenersAt<PhantomListener>(world, 0xF0);
    for (int i = list.m_size - 1; i >= 0; i--) {
        PhantomListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->phantomRemoved(phantom);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireConstraintAdded(hkWorld* world, hkConstraintInstance* constraint) {
    hkArray<ConstraintListener*>& list = listenersAt<ConstraintListener>(world, 0xFC);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ConstraintListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->constraintAdded(constraint);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireConstraintRemoved(hkWorld* world, hkConstraintInstance* constraint) {
    hkArray<ConstraintListener*>& list = listenersAt<ConstraintListener>(world, 0xFC);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ConstraintListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->constraintRemoved(constraint);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireContactPointAdded(hkWorld* world, ContactPointEvent* event) {
    event->m_unk0C = 0;
    hkArray<ContactListener*>& list = listenersAt<ContactListener>(world, 0x15C);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ContactListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->contactPointAdded(event);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireContactPointConfirmed(hkWorld* world, ContactPointEvent* event) {
    event->m_unk08 = 0;
    hkArray<ContactListener*>& list = listenersAt<ContactListener>(world, 0x15C);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ContactListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->contactPointConfirmed(event);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireContactPointRemoved(hkWorld* world, ContactPointEvent* event) {
    event->m_unk10 = 0;
    hkArray<ContactListener*>& list = listenersAt<ContactListener>(world, 0x15C);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ContactListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->contactPointRemoved(event);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireContactProcess(hkWorld* world, ContactPointEvent* event) {
    event->m_unk08 = 0;
    hkArray<ContactListener*>& list = listenersAt<ContactListener>(world, 0x15C);
    for (int i = list.m_size - 1; i >= 0; i--) {
        ContactListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->contactProcess(event);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireWorldDeleted(hkWorld* world) {
    hkArray<WorldDeletionListener*>& list = listenersAt<WorldDeletionListener>(world, 0x108);
    for (int i = list.m_size - 1; i >= 0; i--) {
        WorldDeletionListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->worldDeleted(world);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireIslandActivated(hkWorld* world, hkSimulationIsland* island) {
    world->m_lockCount++;
    hkArray<IslandActivationListener*>& list = listenersAt<IslandActivationListener>(world, 0x114);
    for (int i = list.m_size - 1; i >= 0; i--) {
        IslandActivationListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->islandActivated(island);
        }
    }
    removeNullListeners(list);
    for (int i = 0; i < island->m_entities.m_size; i++) {
        hkEntity* entity = island->m_entities.begin()[i];
        hkArray<EntityActivationListener*>& entityList = *(hkArray<EntityActivationListener*>*)&entity->m_activationListeners;
        for (int j = entityList.m_size - 1; j >= 0; j--) {
            EntityActivationListener* listener = entityList.begin()[j];
            if (listener != 0) {
                listener->entityActivated(island->m_entities[i]);
            }
        }
        removeNullListeners(entityList);
    }
    world->m_lockCount--;
    if (world->m_lockCount == 0 && world->m_pendingOperationsCount != 0 && !(bool)world->m_unk84) {
        world->internal_executePendingOperations();
    }
}

void hkWorldCallbackUtil::fireIslandDeactivated(hkWorld* world, hkSimulationIsland* island) {
    world->m_lockCount++;
    hkArray<IslandActivationListener*>& list = listenersAt<IslandActivationListener>(world, 0x114);
    for (int i = list.m_size - 1; i >= 0; i--) {
        IslandActivationListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->islandDeactivated(island);
        }
    }
    removeNullListeners(list);
    for (int i = 0; i < island->m_entities.m_size; i++) {
        hkEntity* entity = island->m_entities.begin()[i];
        hkArray<EntityActivationListener*>& entityList = *(hkArray<EntityActivationListener*>*)&entity->m_activationListeners;
        for (int j = entityList.m_size - 1; j >= 0; j--) {
            EntityActivationListener* listener = entityList.begin()[j];
            if (listener != 0) {
                listener->entityDeactivated(island->m_entities[i]);
            }
        }
        removeNullListeners(entityList);
    }
    world->m_lockCount--;
    if (world->m_lockCount == 0 && world->m_pendingOperationsCount != 0 && !(bool)world->m_unk84) {
        world->internal_executePendingOperations();
    }
}

void hkWorldCallbackUtil::firePostSimulationCallback(hkWorld* world) {
    hkArray<PostSimulationListener*>& list = listenersAt<PostSimulationListener>(world, 0x120);
    for (int i = list.m_size - 1; i >= 0; i--) {
        PostSimulationListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->postSimulationCallback(world);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireInactiveEntityMoved(hkWorld* world, hkEntity* entity) {
    hkArray<PostSimulationListener*>& list = listenersAt<PostSimulationListener>(world, 0x120);
    for (int i = list.m_size - 1; i >= 0; i--) {
        PostSimulationListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->inactiveEntityMoved(entity);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::firePostIntegrateCallback(hkWorld* world, void* arg) {
    hkArray<PostIntegrateListener*>& list = listenersAt<PostIntegrateListener>(world, 0x12C);
    for (int i = list.m_size - 1; i >= 0; i--) {
        PostIntegrateListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->postIntegrateCallback(world, arg);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::firePostCollideCallback(hkWorld* world, void* arg) {
    hkArray<PostCollideListener*>& list = listenersAt<PostCollideListener>(world, 0x138);
    for (int i = list.m_size - 1; i >= 0; i--) {
        PostCollideListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->postCollideCallback(world, arg);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireIslandPostIntegrateCallback(hkWorld* world, void* a, void* b) {
    hkArray<IslandPostListener*>& list = listenersAt<IslandPostListener>(world, 0x144);
    for (int i = list.m_size - 1; i >= 0; i--) {
        IslandPostListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->islandPostCallback(a, b);
        }
    }
    removeNullListeners(list);
}

void hkWorldCallbackUtil::fireIslandPostCollideCallback(hkWorld* world, void* a, void* b) {
    hkArray<IslandPostListener*>& list = listenersAt<IslandPostListener>(world, 0x150);
    for (int i = list.m_size - 1; i >= 0; i--) {
        IslandPostListener* listener = list.begin()[i];
        if (listener != 0) {
            listener->islandPostCallback(a, b);
        }
    }
    removeNullListeners(list);
}

// Instantiations of the hkArray<hkConstraintInternal> insertion helpers.
template void hkArray<hkConstraintInternal>::insertAt(int i, const hkConstraintInternal& t);
template void hkArray<hkConstraintInternal>::insertAt(int i, const hkArray<hkConstraintInternal>& other);

// fn_802F58CC: body from https://github.com/jj-link/brawl src/havok/fn_802F58CC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802F58CC {
struct Node { u32 pad0; s16 value; u16 pad6; Node* next; Node* other; s16 otherValue; };
struct Object { char pad0[0x10]; Node* head; char pad14[0x14]; Node* tail; };
// TODO: cleanup (shape from jj-link/brawl fn_802F58CC)
extern "C" void fn_802F58CC(Object* obj, Node* oldNode, Node* newNode, s16 value) {
    Node* tail;
    Node* node;
    node = obj->head;
    tail = obj->tail;
    if (node == 0) {
        if (tail != 0) { tail->other = newNode; tail->otherValue = value; }
    } else if (node == oldNode) {
        obj->head = newNode;
        if (tail != 0) { tail->other = newNode; tail->otherValue = value; }
    } else {
    loop:
        Node* next = node->next;
        if (next != oldNode) {
            node = next;
            goto loop;
        }
        node->next = newNode;
        node->value = value;
    }
}
} // namespace jj_fn_802F58CC
