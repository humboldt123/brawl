#pragma once

#include <StaticAssert.h>
#include <havok/hkArray.h>
#include <nw4r/math/math_types.h>

struct ph2ndaryLine;

// HYPOTHESIS: owned link object of a line; its first word is the parent line (setLocalSpace reads the parent's
// local matrix through it, the destructor deletes the link only when that word is null).
struct phOwnedBlock {
    ~phOwnedBlock() {} // HYPOTHESIS: inline destructor (the delete keeps its own null check)
    ph2ndaryLine* m_parent; // 0x00
};

// Secondary line constraint (a chain of nodes driven by the 2ndary controller). Only the members touched by
// the destructor and the matrix updates are recovered so far; the rest of the layout is unknown.
struct ph2ndaryLine {
    ph2ndaryLine();   // not written yet
    ~ph2ndaryLine();

    void updateMatrixForMyPosture();
    void updateMatrixForMotion(const nw4r::math::MTX34* mtx);
    void updateMatrix(const nw4r::math::MTX34* mtx, nw4r::math::VEC3* tip, nw4r::math::VEC3* root);

    u8 unk00[0x1C];                 // 0x00
    nw4r::math::VEC3 unk1C;         // 0x1C, tip position written by updateMatrixForMotion
    nw4r::math::VEC3 unk28;         // 0x28, root position written by updateMatrixForMotion
    nw4r::math::VEC3 unk34;         // 0x34, tip position written by updateMatrixForMyPosture
    nw4r::math::VEC3 unk40;         // 0x40, root position written by updateMatrixForMyPosture
    u8 unk4C[0x80 - 0x4C];          // 0x4C
    f32 unk80;                      // 0x80, offset of the tip along the local x axis
    f32 unk84;                      // 0x84, tip-to-root distance written by the matrix updates
    u8 unk88[0x90 - 0x88];          // 0x88
    char m_nodeName[0xDC - 0x90];   // 0x90, model node name looked up by get2ndaryLastNode (HYPOTHESIS: extent)
    nw4r::math::MTX34 unkDC;        // 0xDC, own posture matrix used by updateMatrixForMyPosture
    u8 unk10C[0x1A0 - 0x10C];       // 0x10C
    u32 m_id;                       // 0x1A0, line id matched by setAwake
    u8 unk1A4[0x1C4 - 0x1A4];       // 0x1A4
    u8 m_winding;                   // 0x1C4, cleared by ph2ndaryController::resetWinding
    u8 unk1C5[0x1F8 - 0x1C5];       // 0x1C5
    phOwnedBlock* m_block;          // 0x1F8 HYPOTHESIS: owned object, deleted when its first word is 0
    hkArray<u32> m_array;           // 0x1FC freed with class 0x15 (HYPOTHESIS: 4-byte element type)
};
static_assert(sizeof(ph2ndaryLine) == 0x208, "ph2ndaryLine size");
