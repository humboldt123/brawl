// Havok translation unit hkMatrix6.o (main.dol 0x80283EB4-0x80284DB0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80283EB4   908  hkMatrix6Sub   [map: hkMatrix6__hkMatrix6Sub]
//   0x80284240   360  hkMatrix6SetMul   [map: hkMatrix6__hkMatrix6SetMul]
//   0x802843A8   620  hkMatrix6SetMulV   [map: hkMatrix6__hkMatrix6SetMulV]
//   0x80284614    92  hkMatrix6SetTranspose   [map: hkMatrix6__hkMatrix6SetTranspose]
//   0x80284670  1856  hkMatrix6SetInvert   [map: hkMatrix6__hkMatrix6SetInvert]
#include <havok/hkMatrix6.h>

// hkMatrix3 has its own header, but that header's hkRotation clashes with include/havok/hkRotation.h,
// so only the one member used here is declared locally (same storage: 12 floats).
struct hkMatrix3 {
    void setTranspose(const hkMatrix3& other);
};

static hkMatrix3& asMatrix3(hkReal* p) { return *reinterpret_cast<hkMatrix3*>(p); }
static const hkMatrix3& asMatrix3(const hkReal* p) { return *reinterpret_cast<const hkMatrix3*>(p); }

// Transpose of the block grid: diagonal blocks transpose in place, off-diagonal blocks swap.
void hkMatrix6::hkMatrix6SetTranspose(const hkMatrix6& other) {
    asMatrix3(&m_blocks[0]).setTranspose(asMatrix3(&other.m_blocks[0]));
    asMatrix3(&m_blocks[12]).setTranspose(asMatrix3(&other.m_blocks[24]));
    asMatrix3(&m_blocks[24]).setTranspose(asMatrix3(&other.m_blocks[12]));
    asMatrix3(&m_blocks[36]).setTranspose(asMatrix3(&other.m_blocks[36]));
}

// Not yet decompiled in this unit:
//   0x80283EB4   908  hkMatrix6Sub   [map: hkMatrix6__hkMatrix6Sub]
//   0x80284240   360  hkMatrix6SetMul   [map: hkMatrix6__hkMatrix6SetMul]
//   0x802843A8   620  hkMatrix6SetMulV   [map: hkMatrix6__hkMatrix6SetMulV]
