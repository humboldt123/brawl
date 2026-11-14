// Havok translation unit hkEntityCallbackUtil.o (main.dol 0x802E3CD4-0x802E4338).
// Functions in address order (method names from the Havok TU map where it has them):
//   0x802E3CD4   124  fireEntityAdded
//   0x802E3D50   256  fireEntityRemoved
//   0x802E3E50   124  fireEntityShapeSet
//   0x802E3ECC   124  fireEntityDeleted
//   0x802E3F48   252  fireContactPointAddedInternal
//   0x802E4044   252  fireContactPointConfirmedInternal   [map: hkEntityCallbackUtil__fireContactPointConfirmedInternal]
//   0x802E4140   252  fireContactPointRemovedInternal   [map: hkEntityCallbackUtil__fireContactPointRemovedInternal]
//   0x802E423C   252  fireContactProcessInternal   [map: hkEntityCallbackUtil__fireContactProcessInternal]
// HYPOTHESIS: the names of the first five (unnamed in the map; the listener slot each one calls, and
// fireEntityRemoved detaching the entity's constraints and actions).

#include <havok/hkEntity.h>
#include <havok/hkBase.h>
#include <havok/hkEntityCallbackUtil.h>
#include <havok/hkConstraintInstance.h>
#include <havok/hkAction.h>

// Entity listener interface (slots 0x0C..0x18 after the destructor).
struct hkEntityListener {
    virtual ~hkEntityListener();
    virtual void entityAddedCallback(hkEntity* entity);    // 0x0C
    virtual void entityRemovedCallback(hkEntity* entity);  // 0x10
    virtual void entityShapeSetCallback(hkEntity* entity); // 0x14
    virtual void entityDeletedCallback(hkEntity* entity);  // 0x18
};

// Collision listener interface: four callbacks from slot 0x08 (no virtual destructor).
struct hkCollisionListener {
    virtual void contactPointAdded(hkContactPointAddedEvent* ev);               // 0x08
    virtual void contactPointConfirmed(struct hkCollisionEventConfirmed* ev); // 0x0C
    virtual void contactPointRemoved(struct hkCollisionEventRemoved* ev);     // 0x10
    virtual void contactProcess(struct hkCollisionEventConfirmed* ev);        // 0x14
};

// HYPOTHESIS: event records; the entity pointer sits at 0x08 (confirmed/process) or 0x10 (removed).
struct hkCollisionEventConfirmed {
    u8 unk00[0x08];
    hkEntity* m_entity; // 0x08
};

struct hkCollisionEventRemoved {
    u8 unk00[0x10];
    hkEntity* m_entity; // 0x10
};

void hkEntityCallbackUtil::fireEntityAdded(hkEntity* entity) {
    for (int i = entity->m_entityListeners.getSize() - 1; i >= 0; i--) {
        hkEntityListener* listener = entity->m_entityListeners[i];
        if (listener) {
            listener->entityAddedCallback(entity);
        }
    }
}

// Also detaches everything still attached to the entity: each callback removes the constraint or action from the
// entity's arrays, so the first element is taken until they are empty.
void hkEntityCallbackUtil::fireEntityRemoved(hkEntity* entity) {
    for (int i = entity->m_entityListeners.getSize() - 1; i >= 0; i--) {
        hkEntityListener* listener = entity->m_entityListeners[i];
        if (listener) {
            listener->entityRemovedCallback(entity);
        }
    }
    while (entity->m_constraintsMaster.getSize()) {
        ((hkConstraintInstance*)entity->m_constraintsMaster[0].m_unk00)->entityRemovedCallback(entity);
    }
    while (entity->m_constraintsSlave.getSize()) {
        entity->m_constraintsSlave[0]->entityRemovedCallback(entity);
    }
    while (entity->m_actions.getSize()) {
        entity->m_actions[0]->entityRemovedCallback(entity);
    }
}

void hkEntityCallbackUtil::fireEntityShapeSet(hkEntity* entity) {
    for (int i = entity->m_entityListeners.getSize() - 1; i >= 0; i--) {
        hkEntityListener* listener = entity->m_entityListeners[i];
        if (listener) {
            listener->entityShapeSetCallback(entity);
        }
    }
}

void hkEntityCallbackUtil::fireEntityDeleted(hkEntity* entity) {
    for (int i = entity->m_entityListeners.getSize() - 1; i >= 0; i--) {
        hkEntityListener* listener = entity->m_entityListeners[i];
        if (listener) {
            listener->entityDeletedCallback(entity);
        }
    }
}

// compacts a listener list by dropping its null entries (the same shape as hkWorldCallbackUtil's)
template <typename T>
static inline void removeNullListeners(hkArray<T*>& list) {
    for (int i = list.m_size - 1; i >= 0; i--) {
        if (list.begin()[i] == 0) {
            list.removeAtAndCopy(i);
        }
    }
}

void hkEntityCallbackUtil::fireContactPointAddedInternal(hkEntity* entity, hkContactPointAddedEvent* ev) {
    ev->m_callbackFiredFrom = entity;
    int n = entity->m_collisionListeners.getSize();
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = entity->m_collisionListeners[i];
        if (listener) {
            listener->contactPointAdded(ev);
        }
    }
    // Listeners removed during the callbacks left null slots: drop them now, keeping the order.
    removeNullListeners(entity->m_collisionListeners);
}

void hkEntityCallbackUtil::fireContactPointConfirmedInternal(hkEntity* entity, hkCollisionEventConfirmed* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.getSize();
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = entity->m_collisionListeners[i];
        if (listener) {
            listener->contactPointConfirmed(ev);
        }
    }
    // Listeners removed during the callbacks left null slots: drop them now, keeping the order.
    removeNullListeners(entity->m_collisionListeners);
}

void hkEntityCallbackUtil::fireContactPointRemovedInternal(hkEntity* entity, hkCollisionEventRemoved* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.getSize();
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = entity->m_collisionListeners[i];
        if (listener) {
            listener->contactPointRemoved(ev);
        }
    }
    // Listeners removed during the callbacks left null slots: drop them now, keeping the order.
    removeNullListeners(entity->m_collisionListeners);
}

void hkEntityCallbackUtil::fireContactProcessInternal(hkEntity* entity, hkCollisionEventConfirmed* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.getSize();
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = entity->m_collisionListeners[i];
        if (listener) {
            listener->contactProcess(ev);
        }
    }
    // Listeners removed during the callbacks left null slots: drop them now, keeping the order.
    removeNullListeners(entity->m_collisionListeners);
}
