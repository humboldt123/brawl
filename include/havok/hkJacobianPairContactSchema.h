#pragma once

#include <havok/hkBase.h>

// Pair contact schema: tag word 0x040C0008 and one float at 0x04.
struct hkJacobianPairContactSchema {
    u32 m_tag;  // 0x00
    float unk04; // 0x04

    void initPairContact(float f);
};
