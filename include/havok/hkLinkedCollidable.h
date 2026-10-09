#pragma once

#include <havok/hkCollidable.h>
#include <havok/hkArray.h>

// Collidable with a list of collision entries (0x30 bytes). Layout from hkLinkedCollidableClass.cpp.
struct hkLinkedCollidable : hkCollidable {
    // hkArray<hkLinkedCollidableCollisionEntry> in the original (8-byte entries, see the array helper
    // unit hk_array_q218hk_linked_collidable14_collision_entry.cpp). Untyped here so owners' constructors
    // do not construct it implicitly; the owners write its three words.
    hkArrayBase m_collisionEntries; // 0x24

    // Element type of m_collisionEntries (8 bytes). HYPOTHESIS: two words, names not recovered.
    struct CollisionEntry {
        u32 m_unk00; // 0x00
        u32 m_unk04; // 0x04
    };
};
