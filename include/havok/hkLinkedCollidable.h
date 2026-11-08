#pragma once

#include <havok/hkCollidable.h>
#include <havok/hkArray.h>

struct hkAgentNnEntry;

// Collidable with a list of collision entries (0x30 bytes). Layout from hkLinkedCollidableClass.cpp.
struct hkLinkedCollidable : hkCollidable {
    // Element type of m_collisionEntries (8 bytes): one agent entry per collision partner.
    // HYPOTHESIS: the names, from hkSimulation::processAgentsOfEntities (it runs the agent entry and looks the
    // partner up among the linked collidables it has already done).
    struct CollisionEntry {
        hkAgentNnEntry* m_agentEntry;  // 0x00
        hkLinkedCollidable* m_partner; // 0x04
    };

    hkArray<CollisionEntry> m_collisionEntries; // 0x24 (see the array helper unit
                                                // hk_array_q218hk_linked_collidable14_collision_entry.cpp)

    hkLinkedCollidable(hkShape* shape, int type) : hkCollidable(shape, type) {}
    hkLinkedCollidable(hkFinishLoadedObjectFlag flag) : hkCollidable(flag) {}
};
