#pragma once

#include <types.h>

// 8 bytes of work memory the hit objects of the grounds allocate (HYPOTHESIS: filled in by the module builder).
struct grPiratesAttackWork {
    u32 unk0;
    u32 unk4;
    grPiratesAttackWork() : unk0(0), unk4(0) { }
};

// Plain storage for a matrix of the scene (constructed with setIdentity by the owner, not through Matrix's own constructor).
struct stPiratesMtx {
    float m[3][4];
};
