#pragma once

#include <havok/hkBase.h>

// Goto schema (no members used so far).
struct hkJacobianGotoSchema {
    template <class T>
    T* hkAddByteOffset(int off) const;
};
