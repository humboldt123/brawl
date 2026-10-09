#pragma once

#include <havok/hkBase.h>

// Eight-float vector (two 4-float halves).
struct hkVector8 {
    float m_v[8];

    void setZero8();
    void setSub8(const hkVector8& a, const hkVector8& b);
};
