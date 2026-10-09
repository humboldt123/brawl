#pragma once

#include <havok/hkBase.h>

// Chain solver info: the first word is set by the constructor.
struct hkChainSolverInfo {
    u32 m_word00; // 0x00 HYPOTHESIS: name

    hkChainSolverInfo(u32 v);
};
