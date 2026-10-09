#pragma once

#include <havok/hkQsTransform.h>

// HYPOTHESIS: hkSkeleton fields used by the hkPose TU (layout from src/havok/hkSkeletonClass.cpp). The
// reflection class is not matched yet, so only the members the pose code reads are named.
struct hkSkeleton {
    const char* m_name;             // 0x00
    s16* m_parentIndices;           // 0x04 (hkSimpleArray<hkInt16> data)
    int m_parentIndicesSize;        // 0x08
    void* m_bones;                  // 0x0C
    int m_numBones;                 // 0x10
    hkQsTransform* m_referencePose; // 0x14
};
