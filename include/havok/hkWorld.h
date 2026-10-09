#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkVector4.h>

struct hkWorldOperationQueue;
struct hkWorldDeletionListener;
struct hkWorldMemoryWatchDog;
struct hkPhantom;
struct hkAction;
struct hkEntity;
struct hkConstraintInstance;
struct hkPhysicsSystem;
struct hkCollisionFilter;
struct hkStatisticsCollector;

// The simulation world (layout partly recovered from hkWorld.cpp asm; the tail after 0x114 is not
// recovered). Fields marked unk are written by the constructor and not read by the code matched so far.
struct hkWorld : hkReferencedObject {
    void* m_unk08;                                  // 0x08 HYPOTHESIS: broadphase-like object (vtable slot 0x10 queried)
    hkVector4 m_gravity;                            // 0x10
    u8 m_unk20[0x08];                               // 0x20 .. 0x27 not recovered
    hkArrayBase m_unk28;                            // 0x28 array of hkWorldMemoryEntry* (HYPOTHESIS: islands), see getMemUsageForIntegration
    hkArrayBase m_unk34;                            // 0x34 not recovered
    hkArrayBase m_unk40;                            // 0x40 not recovered
    u8 m_unk4C[0x28];                               // 0x4C .. 0x73 not recovered
    hkWorldOperationQueue* m_operationQueue;        // 0x74 (allocated 0x20 bytes in the constructor)
    int m_unk78;                                    // 0x78 nonzero means pending operations may exist
    int m_lockCount;                                // 0x7C incremented around add/remove calls
    int m_unk80;                                    // 0x80 summed with m_lockCount in removePhantom
    u8 m_unk84;                                     // 0x84 flag: operations are being executed
    u8 m_unk85;                                     // 0x85 (initialised to 1)
    int m_unk88;                                    // 0x88
    int m_unk8C;                                    // 0x8C (initialised to 1)
    int m_unk90;                                    // 0x90
    u8 m_unk94[0x08];                               // 0x94 .. 0x9B not recovered
    int m_unk9C;                                    // 0x9C
    int m_unkA0;                                    // 0xA0
    int m_unkA4;                                    // 0xA4
    u8 m_unkA8[0x20];                               // 0xA8 .. 0xC7 not recovered
    int m_unkC8;                                    // 0xC8 (initialised to -1)
    hkArray<hkPhantom*> m_phantoms;                 // 0xCC
    u8 m_unkD8[0x30];                               // 0xD8 .. 0x107 not recovered
    hkArray<hkWorldDeletionListener*> m_worldDeletionListeners; // 0x108

    int getMemUsageForIntegration();
    void setGravity(const hkVector4& gravity);
    void queueOperation(const void* operation);
    void addWorldDeletionListener(hkWorldDeletionListener* listener);
    void removeWorldDeletionListener(hkWorldDeletionListener* listener);
    void removeAction(hkAction* action);
    void removeActionImmediately(hkAction* action);
    void addEntityBatch(hkEntity** entities, int count, bool active);
    void removeEntity(hkEntity* entity);
    void removeEntityBatch(hkEntity** entities);
    void addAction(hkAction* action);
    void addConstraint(hkConstraintInstance* constraint);
    void removeConstraint(hkConstraintInstance* constraint);
    void addPhantomBatch(hkPhantom** phantoms, int count);
    void removePhantomBatch(hkPhantom** phantoms, int count);
    void addPhysicsSystem(hkPhysicsSystem* system);
    void removePhysicsSystem(hkPhysicsSystem* system);
    void lock();
    void unlock();
    void lockIslandForConstraintUpdate();
    void unlockIslandForConstraintUpdate();
};
