#pragma once

#include <havok/hkBase.h>

// 3d friction schema: tag word 0x090D001C and a float at 0x18.
struct hkJacobian3dFrictionSchema {
    u32 m_tag;   // 0x00
    u8 unk04[0x18 - 0x04];
    float unk18; // 0x18

    void initAngular(float f);
};
