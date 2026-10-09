#pragma once

#include <havok/hkBase.h>

// Jacobian schema header: the first word is a schema tag (0x01010018 here), followed by five words.
struct hkJacobianHeaderSchema {
    u32 m_tag;  // 0x00
    u32 unk04;  // 0x04
    u32 unk08;  // 0x08
    u32 unk0C;  // 0x0C
    u32 unk10;  // 0x10
    u32 unk14;  // 0x14

    void initHeader(u32 a, u32 b, u32 c, u32 d, u32 e);
};
