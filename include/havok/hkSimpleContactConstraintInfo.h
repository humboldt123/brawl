#pragma once

#include <havok/hkBase.h>

struct hkSimpleContactConstraintInfo {
    // Byte distance from base to ptr (the static helper used by the Jacobian builder).
    static int hkGetByteOffset(const void* base, const void* ptr);
};
