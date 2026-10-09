#pragma once

#include <havok/hkBase.h>

// Single contact schema: tag word 0x03090004.
struct hkJacobianSingleContactSchema {
    u32 m_tag; // 0x00

    void initSingleContact();
};
