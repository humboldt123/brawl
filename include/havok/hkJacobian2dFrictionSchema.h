#pragma once

#include <havok/hkBase.h>

// 2d friction schema: tag word 0x080C0018, a word at 0x04 and 0x14, floats at 0x08 and 0x0C, and 1.0f at 0x10.
struct hkJacobian2dFrictionSchema {
    u32 m_tag;   // 0x00
    u32 unk04;   // 0x04
    float unk08; // 0x08
    float unk0C; // 0x0C
    float unk10; // 0x10
    u32 unk14;   // 0x14

    void init2dFriction(u32 a, u32 b, float c, float d);
};
