#pragma once

#include <havok/hkVector4.h>

// Axis-aligned bounding box (0x20 bytes). Layout from hkAabbClass.cpp.
struct hkAabb {
    hkVector4 m_min; // 0x00
    hkVector4 m_max; // 0x10
};
