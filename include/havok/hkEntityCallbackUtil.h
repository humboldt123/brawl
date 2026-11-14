#pragma once

#include <havok/hkBase.h>

struct hkEntity;
struct hkCollisionEventConfirmed;
struct hkCollisionEventRemoved;
struct hkCdBody;
struct hkProcessCollisionOutput;
struct hkContactPoint;
struct hkContactPointMaterial;
struct hkContactMgr;

// Event handed to the contact-point-added callbacks (0x2C bytes on the stack). HYPOTHESIS: layout beyond the
// stores; the callbacks may change the projected velocity and set the status returned by addToi.
struct hkContactPointAddedEvent {
    const hkCdBody* m_bodyA;               // 0x00
    const hkCdBody* m_bodyB;               // 0x04
    void* unk08;                           // 0x08
    hkEntity* m_callbackFiredFrom;         // 0x0C (set by the entity callbacks)
    hkProcessCollisionOutput* m_output;    // 0x10
    hkContactPoint* m_contactPoint;        // 0x14
    hkContactPointMaterial* m_material;    // 0x18
    float m_projectedVelocity;             // 0x1C
    int m_status;                          // 0x20
    hkContactMgr* m_contactMgr;            // 0x24
    float m_toi;                           // 0x28
};

// Calls the listeners registered on an entity (map names hkEntityCallbackUtil__*).
struct hkEntityCallbackUtil {
    static void fireEntityAdded(hkEntity* entity);
    static void fireEntityRemoved(hkEntity* entity);
    static void fireEntityShapeSet(hkEntity* entity);
    // hkEntity's destructor calls it first.
    static void fireEntityDeleted(hkEntity* entity);
    static void fireContactPointAddedInternal(hkEntity* entity, hkContactPointAddedEvent* ev);
    static void fireContactPointConfirmedInternal(hkEntity* entity, hkCollisionEventConfirmed* ev);
    static void fireContactPointRemovedInternal(hkEntity* entity, hkCollisionEventRemoved* ev);
    static void fireContactProcessInternal(hkEntity* entity, hkCollisionEventConfirmed* ev);
};
