// Havok translation unit hkWorldAgentUtil.o (main.dol 0x802F34F0-0x802F5D5C).
// Work in progress: hkWorldCallbackUtil part. Functions in address order (map names in brackets).
//   0x802F39A0   fireActionAdded, fireActionRemoved, fireEntityAdded, fireEntityRemoved, fireEntityShapeSet,
//   firePhantomAdded, firePhantomRemoved, fireConstraintAdded, fireConstraintRemoved (248 bytes each)

#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkWorldConstraintUtil.h>
#include <havok/hkEntity.h>
#include <havok/hkSimulationIsland.h>

template <typename T>
void hkWorldCallbackUtil::removeNullListeners(hkArray<T*>& list) {
    for (int i = list.m_size - 1; i >= 0; i--) {
        if (list.begin()[i] == 0) {
            list.m_size--;
            for (int j = i; j < list.m_size; j++) {
                list.begin()[j] = list.begin()[j + 1];
            }
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
    if (world->m_lockCount == 0 && world->m_unk78 != 0 && (s8)world->m_unk84 == 0) {
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
    if (world->m_lockCount == 0 && world->m_unk78 != 0 && (s8)world->m_unk84 == 0) {
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
