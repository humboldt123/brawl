#pragma once

#include <havok/hkTransform.h>

// Vector helpers on arrays (Havok hkVector4Util). Only the members a written caller needs are declared.
struct hkVector4Util {
    // vOut[i] = t * (vIn[i].xyz, 1) for i < numVectors (out-of-line copy in hkCollideCapsuleUtil.o, not written yet).
    // HYPOTHESIS: the lane layout of the result (the caller reads x, y, z and w of each output).
    static void mul4xyz1Points(const hkTransform& t, const hkVector4* vIn, int numVectors, hkVector4* vOut);
};
