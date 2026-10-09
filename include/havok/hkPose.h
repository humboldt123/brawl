#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkQsTransform.h>

struct hkSkeleton; // layout used by this TU is declared in src/havok/hkPose.cpp (no header yet)

// Pose of a skeleton: local (bone-relative) and model (skeleton-root) transforms plus per-bone
// dirty flags. Layout from the hkPose TU; the names are HYPOTHESIS (no reflection class exists).
struct hkPose {
    enum BoneFlag {
        BONE_FLAG_MODEL_WRITTEN = 0x1,  // HYPOTHESIS: model transform written by the user
        BONE_FLAG_LOCAL_DIRTY = 0x2,    // HYPOTHESIS: local transform changed, model must be recomputed
        BONE_FLAG_ACCESSED = 0x4,       // HYPOTHESIS: bone accessed in writable mode
        BONE_FLAG_PARENT_PENDING = 0x8, // HYPOTHESIS: on the path of a bone being recomputed
    };

    hkSkeleton* m_skeleton;             // 0x00
    hkArray<hkQsTransform> m_localPose; // 0x04
    hkArray<hkQsTransform> m_modelPose; // 0x10
    hkArray<u8> m_boneFlags;            // 0x1C
    hkBool m_modelPoseValid;            // 0x28 HYPOTHESIS: model pose is up to date
    hkBool unk29;                       // 0x29

    hkQsTransform* calculateBoneModelSpace(int boneIndex);
    hkQsTransform* getBoneModelSpace(int boneIndex);
    hkArray<hkQsTransform>& getPoseModelSpace();
    void syncModelSpace();
    hkQsTransform* accessBoneModelSpace(int boneIndex, int mode);
    hkArray<hkQsTransform>& accessPoseModelSpace();
    hkArray<hkQsTransform>& writeAccessPoseModelSpace();
    void setToReferencePose();
};
