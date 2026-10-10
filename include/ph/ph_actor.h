#pragma once

#include <havok/hkBase.h>

struct phShape;

// Entry of the actor's shape array (8 bytes): a key and the owned shape.
struct phActorEntry {
    u32 m_key;       // 0x00 HYPOTHESIS: identifier compared when entries are removed
    phShape* m_shape; // 0x04
};

// Physics actor: owns an array of shapes (data at 0x00, count at 0x04, capacity/flags at 0x08).
struct phActor {
    phActor();
    ~phActor();

    // Draws every owned shape through phShape's virtual draw (vtable slot 0x08).
    void draw();

    phActorEntry* m_entries;   // 0x00
    int m_count;               // 0x04
    u32 m_capacityAndFlags;    // 0x08 bit 31 = data not owned
    u32 unkC;                  // 0x0C
};
