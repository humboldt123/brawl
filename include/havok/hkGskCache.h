#pragma once

#include <havok/hkBase.h>

// GSK cache entry (12 bytes, map name hkGskCache). The fields are not identified yet; the 0x00..0x07 halfwords and the
// 0x08..0x0B bytes are copied together by the assignment.
struct hkGskCache {
    u16 unk0; // 0x00
    u16 unk2; // 0x02
    u16 unk4; // 0x04
    u16 unk6; // 0x06
    u8 unk8;  // 0x08
    u8 unk9;  // 0x09
    u8 unkA;  // 0x0A
    u8 unkB;  // 0x0B

    // Copies the 12 bytes of other (map name hkGskCache____as).
    void operator=(const hkGskCache& other);
};
