#pragma once

#include <havok/hkBase.h>

// SPU padding wrapper: a single scalar or pointer member with assignment and conversion operators.
// Only the PC-side members used by the solver are declared (the target instantiates these from the solver TU).
template <typename T>
struct hkPadSpu {
    T m_value;

    hkPadSpu& operator=(T v) {
        m_value = v;
        return *this;
    }
    operator T() const { return m_value; }
    operator T*() { return &m_value; }
};
