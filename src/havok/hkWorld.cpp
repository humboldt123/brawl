// Havok translation unit hkWorld.o (main.dol 0x802EA88C-0x802F004C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802EA88C  2140  __dt   [map: hkWorld____dt]
//   0x802EB0E8    92  __dt   [map: hkEntityEntityBroadPhaseListener____dt]
//   0x802EB144    92  __dt   [map: hkBroadPhaseBorderListener____dt]
//   0x802EB1A0   512  hkWorld_setupContactMgrFactories   [map: hkWorld__hkWorld_setupContactMgrFactories]
//   0x802EB3A0    92  __dt   [map: hkNullContactMgr____dt]
//   0x802EB3FC  1044  updateCollisionFilterOnWorld   [map: hkWorld__updateCollisionFilterOnWorld]
//   0x802EB810   364  hkWorld_updateFilterOnSinglePhantom   [map: hkPhantom__hkWorld_updateFilterOnSinglePhantom]
//   0x802EB97C   820  updateCollisionFilterOnPhantom   [map: hkWorld__updateCollisionFilterOnPhantom]
//   0x802EBCB0  1296  updateCollisionFilterOnEntity   [map: hkWorld__updateCollisionFilterOnEntity]
//   0x802EC1C0   608  addEntity   [map: hkWorld__addEntity]
//   0x802EC420  1952  addEntityBatch   [map: hkWorld__addEntityBatch]
//   0x802ECBC0   552  removeEntity   [map: hkWorld__removeEntity]
//   0x802ECDE8  1232  removeEntityBatch   [map: hkWorld__removeEntityBatch]
//   0x802ED2B8   308  activateRegion   [map: hkWorld__activateRegion]
//   0x802ED3EC   356  addConstraint   [map: hkWorld__addConstraint]
//   0x802ED550   224  removeConstraint   [map: hkWorld__removeConstraint]
//   0x802ED630   492  addAction   [map: hkWorld__addAction]
//   0x802ED81C    68  removeAction   [map: hkWorld__removeAction]
//   0x802ED860   616  removeActionImmediately   [map: hkWorld__removeActionImmediately]
//   0x802EDAC8   296  addPhantom   [map: hkWorld__addPhantom]
//   0x802EDBF0   972  addPhantomBatch   [map: hkWorld__addPhantomBatch]
//   0x802EDFBC   316  removePhantom   [map: hkWorld__removePhantom]
//   0x802EE0F8   812  removePhantomBatch   [map: hkWorld__removePhantomBatch]
//   0x802EE424   268  addPhysicsSystem   [map: hkWorld__addPhysicsSystem]
//   0x802EE530   264  removePhysicsSystem   [map: hkWorld__removePhysicsSystem]
//   0x802EE638    36  setGravity   [map: hkWorld__setGravity]
//   0x802EE65C   108  addWorldDeletionListener   [map: hkWorld__addWorldDeletionListener]
//   0x802EE6C8    80  removeWorldDeletionListener   [map: hkWorld__removeWorldDeletionListener]
//   0x802EE718   276  stepDeltaTime   [map: hkWorld__stepDeltaTime]
//   0x802EE82C  1772  calcStatistics   [map: hkWorld__calcStatistics]
//   0x802EEF18  3620  __ct   [map: hkWorld____ct]
//   0x802EFD3C   308  setCollisionFilter   [map: hkWorld__setCollisionFilter]
//   0x802EFE70   188  internal_executePendingOperations   [map: hkWorld__internal_executePendingOperations]
//   0x802EFF2C     8  queueOperation   [map: hkWorld__queueOperation]
//   0x802EFF34   108  getMemUsageForIntegration   [map: hkWorld__getMemUsageForIntegration]
//   0x802EFFA0     4  lock   [map: hkWorld__lock]
//   0x802EFFA4     4  unlock   [map: hkWorld__unlock]
//   0x802EFFA8     4  lockIslandForConstraintUpdate   [map: hkWorld__lockIslandForConstraintUpdate]
//   0x802EFFAC     4  unlockIslandForConstraintUpdate   [map: hkWorld__unlockIslandForConstraintUpdate]
//   0x802EFFB0     8  createContactMgr   [map: hkNullContactMgrFactory__createContactMgr]
//   0x802EFFB8     4  cleanup   [map: hkNullContactMgr__cleanup]
//   0x802EFFBC     8  reserveContactPoints   [map: hkNullContactMgr__reserveContactPoints]
//   0x802EFFC4     4  processToi   [map: hkNullContactMgr__processToi]
//   0x802EFFC8     4  removeToi   [map: hkNullContactMgr__removeToi]
//   0x802EFFCC     8  addToi   [map: hkNullContactMgr__addToi]
//   0x802EFFD4     4  processContact   [map: hkNullContactMgr__processContact]
//   0x802EFFD8     4  removeContactPoint   [map: hkNullContactMgr__removeContactPoint]
//   0x802EFFDC     8  addContactPoint   [map: hkNullContactMgr__addContactPoint]
//   0x802EFFE4    92  __dt   [map: hkNullContactMgrFactory____dt]
//   0x802F0040    12  __sinit_\hkWorld_cpp   [map: hkWorldcpp____sinit_]

#include <havok/hkWorld.h>
#include <havok/hkPhysicsSystem.h>

// Operation record queued while the world is locked (8 bytes). HYPOTHESIS: field names; the
// type byte selects the deferred operation, the object pointer is its argument.
struct hkWorldOperationRecord {
    u8 m_type;      // 0x00
    void* m_object; // 0x04
};

// Queue entry point (hkWorldOperationQueue unit, not recovered yet).
extern "C" void fn_802F5E44(hkWorldOperationQueue* queue, const void* operation);

void hkWorld::lock() {}

void hkWorld::unlock() {}

void hkWorld::lockIslandForConstraintUpdate() {}

void hkWorld::unlockIslandForConstraintUpdate() {}

void hkWorld::setGravity(const hkVector4& gravity) {
    m_gravity = gravity;
}

#pragma dont_inline on
void hkWorld::queueOperation(const void* operation) { // out of line in the original
    fn_802F5E44(m_operationQueue, operation);
}
#pragma dont_inline reset

void hkWorld::addWorldDeletionListener(hkWorldDeletionListener* listener) {
    m_worldDeletionListeners.pushBack(listener);
}

void hkWorld::removeWorldDeletionListener(hkWorldDeletionListener* listener) {
    // HYPOTHESIS: an entry is cleared in place (no compaction). Not found leaves index -1.
    int index;
    for (index = 0; index < m_worldDeletionListeners.m_size; index++) {
        if (m_worldDeletionListeners[index] == listener) {
            goto found; // MATCH-ONLY: the not-found path below is shared with the store
        }
    }
    index = -1;
found:
    m_worldDeletionListeners[index] = NULL;
}

#pragma dont_inline on
void hkWorld::removeAction(hkAction* action) {
    if (m_lockCount != 0) {
        hkWorldOperationRecord op;
        op.m_type = 0x0B;
        op.m_object = action;
        queueOperation(&op);
    } else {
        removeActionImmediately(action);
    }
}
#pragma dont_inline reset

// HYPOTHESIS: post-add step of the physics system, not recovered (called with the system's active flag).
extern "C" void fn_802F9098(hkWorld* world, hkBool active);

void hkWorld::addPhysicsSystem(hkPhysicsSystem* system) {
    if (system->m_rigidBodies.m_size > 0) {
        addEntityBatch((hkEntity**)system->m_rigidBodies.m_data, system->m_rigidBodies.m_size, system->m_active);
    }
    if (system->m_phantoms.m_size > 0) {
        addPhantomBatch((hkPhantom**)system->m_phantoms.m_data, system->m_phantoms.m_size);
    }
    for (int i = 0; i < system->m_actions.m_size; i++) {
        hkAction* action = ((hkAction**)system->m_actions.m_data)[i];
        if (action != NULL) {
            addAction(action);
        }
    }
    for (int i = 0; i < system->m_constraints.m_size; i++) {
        addConstraint(((hkConstraintInstance**)system->m_constraints.m_data)[i]);
    }
    if (system->m_rigidBodies.m_size > 0) {
        fn_802F9098(this, system->m_active);
    }
}

void hkWorld::removePhysicsSystem(hkPhysicsSystem* system) {
    for (int i = 0; i < system->m_constraints.m_size; i++) {
        removeConstraint(((hkConstraintInstance**)system->m_constraints.m_data)[i]);
    }
    for (int i = 0; i < system->m_actions.m_size; i++) {
        removeAction(((hkAction**)system->m_actions.m_data)[i]);
    }
    int rigidBodyCount = system->m_rigidBodies.m_size;
    if (rigidBodyCount < 4) {
        for (int i = 0; i < system->m_rigidBodies.m_size; i++) {
            removeEntity(((hkEntity**)system->m_rigidBodies.m_data)[i]);
        }
    } else {
        removeEntityBatch((hkEntity**)system->m_rigidBodies.m_data);
    }
    if (system->m_phantoms.m_size > 0) {
        removePhantomBatch((hkPhantom**)system->m_phantoms.m_data, system->m_phantoms.m_size);
    }
}

// Per-entry record of the 0x28 array (HYPOTHESIS: field names and types, only the read offsets are known).
struct hkWorldMemoryEntry {
    u8 m_unk00[0x0C];  // 0x00
    int m_unk0C;       // 0x0C
    int m_unk10;       // 0x10
    int m_unk14;       // 0x14 (scaled by 4)
    u8 m_unk18[0x24];  // 0x18 .. 0x3B
    int m_unk3C;       // 0x3C (scaled by 128)
};

int hkWorld::getMemUsageForIntegration() {
    int result = 0;
    for (int i = m_unk28.m_size - 1; i >= 0; i--) {
        hkWorldMemoryEntry* entry = ((hkWorldMemoryEntry**)m_unk28.m_data)[i];
        int value = entry->m_unk0C + (entry->m_unk3C << 7) + (entry->m_unk14 << 2) + entry->m_unk10 + 0x9C;
        if (result < value) {
            result = value;
        }
    }
    return result;
}


// Null contact manager and its factory. Declared locally (no header yet; the real base is hkContactMgr in
// hkCollisionAgent.h, not included here). Every manager method is a no-op returning a fixed value; the factory
// hands out the manager embedded at +0x08.
struct hkNullContactMgr {
    void cleanup();
    bool reserveContactPoints();
    void processToi();
    void removeToi();
    bool addToi();
    void processContact();
    void removeContactPoint();
    bool addContactPoint();
};

struct hkNullContactMgrFactory {
    u8 m_unk00[0x08]; // 0x00 (not recovered)
    hkNullContactMgr* createContactMgr();
};

hkNullContactMgr* hkNullContactMgrFactory::createContactMgr() {
    return (hkNullContactMgr*)((u8*)this + 0x08);
}

void hkNullContactMgr::cleanup() {}

bool hkNullContactMgr::reserveContactPoints() {
    return false;
}

void hkNullContactMgr::processToi() {}

void hkNullContactMgr::removeToi() {}

bool hkNullContactMgr::addToi() {
    return true;
}

void hkNullContactMgr::processContact() {}

void hkNullContactMgr::removeContactPoint() {}

bool hkNullContactMgr::addContactPoint() {
    return false;
}
