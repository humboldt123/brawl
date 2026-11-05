#pragma once

#include <havok/hkVector4.h>

// Ray cast input (0x28 bytes, 0x30 with the vector alignment). Layout from the copy code in
// hkConvexTranslateShape: ray start and end at 0x00 and 0x10, then two words copied unchanged (HYPOTHESIS: filter
// and user data; the multi-ray agent zeroes both).
struct hkShapeRayCastInput {
    hkVector4 m_from; // 0x00
    hkVector4 m_to;   // 0x10
    u32 unk20;        // 0x20 HYPOTHESIS
    u32 unk24;        // 0x24 HYPOTHESIS
};

// Ray cast result filled by hkShape::castRay. Layout from the stack copy built by the multi-ray agent: the hit
// normal, the fraction of the ray before the hit (starts at 1), and key bookkeeping (HYPOTHESIS: unk18 is a list of
// shape keys, the first starting at -1, and unk38 its fill count).
struct hkShapeRayCastOutput {
    hkVector4 m_normal;   // 0x00
    hkReal m_hitFraction; // 0x10
    int unk14;            // 0x14 starts at -1
    u32 unk18[8];         // 0x18
    int unk38;            // 0x38 starts at 0

    hkShapeRayCastOutput() {
        m_hitFraction = 1.0f;
        unk14 = -1;
        unk18[0] = 0xFFFFFFFF;
        unk38 = 0;
    }
};
