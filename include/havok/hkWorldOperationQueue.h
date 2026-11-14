#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkThreadMemory.h>

struct hkWorld;
struct hkEntity;
struct hkRigidBody;
struct hkPhantom;
struct hkAction;
struct hkConstraintInstance;
struct hkWorldObject;
struct hkShape;
struct hkAabb;
struct hkVector4;

// Nested operation record (20 bytes). The queue sorts these with islandLess.
// Field offsets from the sort and comparison code; only 0x00, 0x04 and 0x08 are used so far.
struct hkWorldOperation {
    // what a queued operation does. numbers line up with the havok 5.1 enum (and with what hkWorld.cpp
    // was already using as bare numbers, like 0x0B for removeAction)
    enum Type {
        OPERATION_ID_INVALID = 0,
        ENTITY_ADD,                              // 0x01
        ENTITY_REMOVE,                           // 0x02
        ENTITY_UPDATE_BROAD_PHASE,               // 0x03
        RIGIDBODY_SET_MOTION_TYPE,               // 0x04
        WORLD_OBJECT_SET_SHAPE,                  // 0x05
        ENTITY_BATCH_ADD,                        // 0x06
        ENTITY_BATCH_REMOVE,                     // 0x07
        CONSTRAINT_ADD,                          // 0x08
        CONSTRAINT_REMOVE,                       // 0x09
        ACTION_ADD,                              // 0x0A
        ACTION_REMOVE,                           // 0x0B
        ISLAND_MERGE,                            // 0x0C
        PHANTOM_ADD,                             // 0x0D
        PHANTOM_REMOVE,                          // 0x0E
        PHANTOM_BATCH_ADD,                       // 0x0F
        PHANTOM_BATCH_REMOVE,                    // 0x10
        PHANTOM_UPDATE_BROAD_PHASE,              // 0x11
        UPDATE_FILTER_ENTITY,                    // 0x12
        UPDATE_FILTER_PHANTOM,                   // 0x13
        UPDATE_FILTER_WORLD,                     // 0x14
        UPDATE_MOVED_BODY_INFO,                  // 0x15
        ENTITY_BATCH_REINTEGRATE_AND_RECOLLIDE,  // 0x16
        RIGIDBODY_SET_POSITION_AND_ROTATION,     // 0x17
        RIGIDBODY_SET_LINEAR_VELOCITY,           // 0x18
        RIGIDBODY_SET_ANGULAR_VELOCITY,          // 0x19
        RIGIDBODY_APPLY_LINEAR_IMPULSE,          // 0x1A
        RIGIDBODY_APPLY_POINT_IMPULSE,           // 0x1B
        RIGIDBODY_APPLY_ANGULAR_IMPULSE,         // 0x1C
        WORLD_OBJECT_ADD_REFERENCE,              // 0x1D
        WORLD_OBJECT_REMOVE_REFERENCE,           // 0x1E
        ACTIVATE_REGION,                         // 0x1F
        ACTIVATE_ENTITY,                         // 0x20
        DEACTIVATE_ENTITY,                       // 0x21
        USER_CALLBACK,                           // 0x22
    };

    // every operation starts with its type byte, then the payload from 0x04. these are the ones
    // queueOperation touches (the rest of the 5.1 list has the same shape, add as needed)
    struct BaseOperation {
        u8 m_type; // 0x00
        BaseOperation() : m_type(0) {}
    };
    struct SingleObjectOperation : BaseOperation {
        void* m_object; // 0x04 entity / constraint / action / phantom / rigid body / world object, depending on m_type
    };
    struct MergeIslands : BaseOperation {
        hkEntity* m_entities[2]; // 0x04
    };
    struct SetWorldObjectShape : BaseOperation {
        hkWorldObject* m_worldObject; // 0x04
        hkShape* m_shape;             // 0x08
    };
    // batch add/remove for entities and phantoms (and reintegrate-and-recollide): a pointer list plus a count
    struct BatchOperation : BaseOperation {
        void** m_objects; // 0x04
        u16 m_count;      // 0x08
    };
    struct UpdatePhantomBP : BaseOperation {
        hkPhantom* m_phantom; // 0x04
        hkAabb* m_aabb;       // 0x08 the queue keeps its own copy
    };

    struct BiggestOperation : BaseOperation {
        // BYTES: the payload is one aggregate after the type byte (0x01-0x03 padding): copies move byte 0, then the
        // payload as a block through one address; the default ctor zeroes the type byte (inplace array ctor loop).
        struct {
            hkEntity* m_entityA; // 0x04 HYPOTHESIS: entity whose simulation island orders the queue
            hkEntity* m_entityB; // 0x08 HYPOTHESIS: second entity, used as tie breaker
            int m_unk0C;         // 0x0C
            int m_unk10;         // 0x10
        };

        // Orders operations by the island key (u16 at island + 0x20), then by the second entity's island.
        static hkBool islandLess(const BiggestOperation& a, const BiggestOperation& b);
    };
};

// Queue of pending world operations (0x20 bytes). The world holds a pointer to it.
struct hkWorldOperationQueue {
    hkArray<hkWorldOperation::BiggestOperation> m_operations0; // 0x00
    hkWorld* m_world;                                          // 0x0C
    hkArray<hkWorldOperation::BiggestOperation> m_operations1; // 0x10
    hkBool m_unk1C;                                            // 0x1C (tested signed: extsb)

    static void operator delete(void* p) {
        hkThreadMemory::s_instance->deallocateChunk(p, 0x20, 0x2d);
    }

    hkWorldOperationQueue(hkWorld* world);
    ~hkWorldOperationQueue();
    // copies the operation into the pending list and grabs a reference on whatever it points at, so the
    // objects can't go away before the operation actually runs
    void queueOperation(const hkWorldOperation::BaseOperation& operation);
    // runs everything queued while the world was locked (merges first, then in the order they were queued)
    void executeAllPending();
};

// Operation record queued while the world is locked (8 bytes). HYPOTHESIS: field names; the
// type byte selects the deferred operation, the object pointer is its argument.
struct hkWorldOperationRecord {
    u8 m_type;      // 0x00
    void* m_object; // 0x04
};
