#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkThreadMemory.h>

struct hkWorld;
struct hkEntity;

// Nested operation record (20 bytes). The queue sorts these with islandLess.
// Field offsets from the sort and comparison code; only 0x00, 0x04 and 0x08 are used so far.
struct hkWorldOperation {
    struct BiggestOperation {
        u8 m_kind;               // 0x00 HYPOTHESIS: operation kind (first byte moved by the sort)
        u8 m_unk01[0x03];        // 0x01
        hkEntity* m_entityA;     // 0x04 HYPOTHESIS: entity whose simulation island orders the queue
        hkEntity* m_entityB;     // 0x08 HYPOTHESIS: second entity, used as tie breaker
        int m_unk0C;             // 0x0C
        int m_unk10;             // 0x10

        // Orders operations by the island key (u16 at island + 0x20), then by the second entity's island.
        static hkBool islandLess(const BiggestOperation& a, const BiggestOperation& b);
    };
};

// Queue of pending world operations (0x20 bytes). The world holds a pointer to it.
struct hkWorldOperationQueue {
    hkArray<hkWorldOperation::BiggestOperation> m_operations0; // 0x00
    hkWorld* m_world;                                          // 0x0C
    hkArray<hkWorldOperation::BiggestOperation> m_operations1; // 0x10
    u8 m_unk1C;                                                // 0x1C

    static void operator delete(void* p) {
        hkThreadMemory::s_instance->deallocateChunk(p, 0x20, 0x2d);
    }

    hkWorldOperationQueue(hkWorld* world);
    ~hkWorldOperationQueue();
};
