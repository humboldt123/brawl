#pragma once

#include <havok/hkBase.h>

// Stiff spring chain schema. The chain count sits at 0x04.
struct hkJacobianStiffSpringChainSchema {
    u32 m_tag;       // 0x00
    u32 m_numChains; // 0x04 HYPOTHESIS: name

    u8* getEnd(u8* base) const;
};
