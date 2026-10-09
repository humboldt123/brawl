#pragma once

#include <havok/hkBase.h>

// Property value (0x08 bytes). Layout from hkPropertyClass.cpp.
struct hkPropertyValue {
    u64 m_data; // 0x00
};

// Keyed property (0x10 bytes). Layout from hkPropertyClass.cpp.
struct hkProperty {
    u32 m_key;               // 0x00
    u32 alignmentPadding;    // 0x04
    hkPropertyValue m_value; // 0x08
};
