#pragma once

#include <havok/hkVector4.h>

// 6x6 matrix stored as four 3x3 blocks of hkMatrix3 storage (0x30 bytes each), laid out as a 2x2 block grid:
// block (0,0) at 0x00, (0,1) at 0x30, (1,0) at 0x60, (1,1) at 0x90 (0xC0 bytes in all).
struct hkMatrix6 {
    hkReal m_blocks[48]; // 0x00

    void hkMatrix6SetTranspose(const hkMatrix6& other);
};
