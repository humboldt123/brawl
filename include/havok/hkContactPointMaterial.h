#pragma once

#include <havok/hkBase.h>

// Per-contact material properties, stored right after the contact point array.
struct hkContactPointMaterial {
    u8 unk00[0x04];
    u16 m_friction; // 0x04 (8.8 fixed point friction)

    u16 getFriction8_8();
};
