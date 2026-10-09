// Havok translation unit hk_entity_callback_util.o (main.dol 0x802E4044-0x802E4338).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E4044   252  fireContactPointConfirmedInternal   [map: hkEntityCallbackUtil__fireContactPointConfirmedInternal]
//   0x802E4140   252  fireContactPointRemovedInternal   [map: hkEntityCallbackUtil__fireContactPointRemovedInternal]
//   0x802E423C   252  fireContactProcessInternal   [map: hkEntityCallbackUtil__fireContactProcessInternal]

#include <havok/hkEntity.h>
#include <havok/hkBase.h>

// HYPOTHESIS: collision listener interface as used by the entity callbacks (vtable slots 3, 4, 5).
struct hkCollisionListener : hkBaseObject {
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

struct hkEntityCallbackUtil {
    static void fireContactPointConfirmedInternal(hkEntity* entity, hkCollisionEventConfirmed* ev);
    static void fireContactPointRemovedInternal(hkEntity* entity, hkCollisionEventRemoved* ev);
    static void fireContactProcessInternal(hkEntity* entity, hkCollisionEventConfirmed* ev);
};

void hkEntityCallbackUtil::fireContactPointConfirmedInternal(hkEntity* entity, hkCollisionEventConfirmed* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.m_size;
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = ((hkCollisionListener**)entity->m_collisionListeners.m_data)[i];
        if (listener) {
            listener->contactPointConfirmed(ev);
        }
    }
    for (int i = entity->m_collisionListeners.m_size - 1; i >= 0; i--) {
        if (((void**)entity->m_collisionListeners.m_data)[i] == 0) {
            entity->m_collisionListeners.m_size--;
            for (int j = i; j < entity->m_collisionListeners.m_size; j++) {
                ((void**)entity->m_collisionListeners.m_data)[j] = ((void**)entity->m_collisionListeners.m_data)[j + 1];
            }
        }
    }
}

void hkEntityCallbackUtil::fireContactPointRemovedInternal(hkEntity* entity, hkCollisionEventRemoved* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.m_size;
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = ((hkCollisionListener**)entity->m_collisionListeners.m_data)[i];
        if (listener) {
            listener->contactPointRemoved(ev);
        }
    }
    for (int i = entity->m_collisionListeners.m_size - 1; i >= 0; i--) {
        if (((void**)entity->m_collisionListeners.m_data)[i] == 0) {
            entity->m_collisionListeners.m_size--;
            for (int j = i; j < entity->m_collisionListeners.m_size; j++) {
                ((void**)entity->m_collisionListeners.m_data)[j] = ((void**)entity->m_collisionListeners.m_data)[j + 1];
            }
        }
    }
}

void hkEntityCallbackUtil::fireContactProcessInternal(hkEntity* entity, hkCollisionEventConfirmed* ev) {
    ev->m_entity = entity;
    int n = entity->m_collisionListeners.m_size;
    for (int i = n - 1; i >= 0; i--) {
        hkCollisionListener* listener = ((hkCollisionListener**)entity->m_collisionListeners.m_data)[i];
        if (listener) {
            listener->contactProcess(ev);
        }
    }
    for (int i = entity->m_collisionListeners.m_size - 1; i >= 0; i--) {
        if (((void**)entity->m_collisionListeners.m_data)[i] == 0) {
            entity->m_collisionListeners.m_size--;
            for (int j = i; j < entity->m_collisionListeners.m_size; j++) {
                ((void**)entity->m_collisionListeners.m_data)[j] = ((void**)entity->m_collisionListeners.m_data)[j + 1];
            }
        }
    }
}
