#pragma once

#include <havok/hkArray.h>
#include <types.h>

// Secondary-motion world (map class ph2ndaryWorld). Offsets come from the asm of
// src/sora/ph/ph_2ndary_world.cpp. Field names are HYPOTHESIS; unkNN marks fields not yet seen in the asm.
class ph2ndaryWorld {
public:
    // One entry of the constraint list: kind byte, owning pointer and an id.
    struct ConstraintEntry {
        u8 kind;   // 0x00
        void* ptr; // 0x04
        s32 id;    // 0x08
    };

    // Returns the id of the new entry, or -1 when ptr is already in the list.
    s32 addConstraintArray(void* ptr, const u8* kind);
    void removeConstraintArray(void* ptr);
    f32 getSamusRandomAccel(f32 a, f32 b, f32 c);

    hkArray<ConstraintEntry> m_constraints; // 0x00: data, size and capacity word
    u8 unk0C[0x48 - 0x0C];                // 0x0C
    f32 m_unk48;                          // 0x48, scale used by getSamusRandomAccel (HYPOTHESIS: a random-accel factor)
    u8 unk4C[0x6C - 0x4C];                // 0x4C
    s32 m_nextId;                         // 0x6C, id given to the next constraint
};
