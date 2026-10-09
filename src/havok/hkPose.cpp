// Havok translation unit hkPose.o (main.dol 0x8032E9B4-0x8032F0A8).
// Functions in address order (method names from the Havok TU map):
//   0x8032E9B4   376  calculateBoneModelSpace   [map: hkPose__calculateBoneModelSpace]
//   0x8032EB2C    48  getPoseModelSpace   [map: hkPose__getPoseModelSpace]
//   0x8032EB5C   312  syncModelSpace   [map: hkPose__syncModelSpace]
//   0x8032EC94   620  accessBoneModelSpace   [map: hkPose__accessBoneModelSpace]
//   0x8032EF00    52  accessPoseModelSpace   [map: hkPose__accessPoseModelSpace]
//   0x8032EF34   252  writeAccessPoseModelSpace   [map: hkPose__writeAccessPoseModelSpace]
//   0x8032F030   120  setToReferencePose   [map: hkPose__setToReferencePose]
#include <havok/hkPose.h>
#include <havok/hkSkeleton.h>
#include <havok/hkString.h>

#include <havok/hkSkeleton.h>

// MATCH-ONLY: the original copies the twelve floats field by field (lfs/stfs), not as one struct.
static void copyQsTransform(hkQsTransform& dst, const hkQsTransform& src) {
    dst.m_translation.x = src.m_translation.x;
    dst.m_translation.y = src.m_translation.y;
    dst.m_translation.z = src.m_translation.z;
    dst.m_translation.w = src.m_translation.w;
    dst.m_rotation.m_vec.x = src.m_rotation.m_vec.x;
    dst.m_rotation.m_vec.y = src.m_rotation.m_vec.y;
    dst.m_rotation.m_vec.z = src.m_rotation.m_vec.z;
    dst.m_rotation.m_vec.w = src.m_rotation.m_vec.w;
    dst.m_scale.x = src.m_scale.x;
    dst.m_scale.y = src.m_scale.y;
    dst.m_scale.z = src.m_scale.z;
    dst.m_scale.w = src.m_scale.w;
}

hkQsTransform* hkPose::calculateBoneModelSpace(int boneIndex) {
    int first = boneIndex;
    while (m_boneFlags[first] & BONE_FLAG_LOCAL_DIRTY) {
        int parent = m_skeleton->m_parentIndices[first];
        if (parent == -1) {
            copyQsTransform(m_modelPose[first], m_localPose[first]);
            m_boneFlags[first] &= ~BONE_FLAG_LOCAL_DIRTY;
            break;
        }
        m_boneFlags[first] |= BONE_FLAG_PARENT_PENDING;
        first = parent;
    }
    for (int i = first + 1; i <= boneIndex; i++) {
        if (m_boneFlags[i] & BONE_FLAG_PARENT_PENDING) {
            m_modelPose[i].setMul(m_modelPose[m_skeleton->m_parentIndices[i]], m_localPose[i]);
            m_boneFlags[i] &= ~BONE_FLAG_PARENT_PENDING;
            m_boneFlags[i] &= ~BONE_FLAG_LOCAL_DIRTY;
        }
    }
    return &m_modelPose[boneIndex];
}

hkArray<hkQsTransform>& hkPose::getPoseModelSpace() {
    syncModelSpace();
    return m_modelPose;
}

void hkPose::syncModelSpace() {
    if (m_modelPoseValid.m_bool != 0) {
        return;
    }
    int n = m_skeleton->m_numBones;
    for (int i = 0; i < n; i++) {
        if (m_boneFlags[i] & BONE_FLAG_LOCAL_DIRTY) {
            int parent = m_skeleton->m_parentIndices[i];
            if (parent != -1) {
                m_modelPose[i].setMul(m_modelPose[parent], m_localPose[i]);
            } else {
                copyQsTransform(m_modelPose[i], m_localPose[i]);
            }
            m_boneFlags[i] &= ~BONE_FLAG_LOCAL_DIRTY;
        }
    }
    m_modelPoseValid = true;
}

hkQsTransform* hkPose::accessBoneModelSpace(int boneIndex, int mode) {
    if (mode == 0) {
        int n = m_skeleton->m_numBones;
        for (int i = boneIndex + 1; i < n; i++) {
            if (m_skeleton->m_parentIndices[i] == boneIndex) {
                if (m_boneFlags[i] & BONE_FLAG_LOCAL_DIRTY) {
                    calculateBoneModelSpace(i);
                }
                m_boneFlags[i] = BONE_FLAG_MODEL_WRITTEN;
                unk29 = false;
            }
        }
    } else {
        m_boneFlags[boneIndex] |= BONE_FLAG_ACCESSED;
        int n = m_skeleton->m_numBones;
        for (int i = boneIndex + 1; i < n; i++) {
            int parent = m_skeleton->m_parentIndices[i];
            if (m_boneFlags[parent] & BONE_FLAG_ACCESSED) {
                if (m_boneFlags[i] & BONE_FLAG_MODEL_WRITTEN) {
                    if (parent == -1) {
                        copyQsTransform(m_localPose[i], m_modelPose[i]);
                    } else {
                        if (m_boneFlags[parent] & BONE_FLAG_LOCAL_DIRTY) {
                            calculateBoneModelSpace(parent);
                        }
                        m_localPose[i].setMulInverseMul(m_modelPose[parent], m_modelPose[i]);
                    }
                    m_boneFlags[i] &= ~BONE_FLAG_MODEL_WRITTEN;
                }
                m_boneFlags[i] |= BONE_FLAG_ACCESSED;
                m_modelPoseValid = false;
            }
        }
        for (int i = boneIndex + 1; i < n; i++) {
            if (m_boneFlags[i] & BONE_FLAG_ACCESSED) {
                m_boneFlags[i] |= BONE_FLAG_LOCAL_DIRTY;
                m_boneFlags[i] &= ~BONE_FLAG_ACCESSED;
            }
        }
    }
    if (m_boneFlags[boneIndex] & BONE_FLAG_LOCAL_DIRTY) {
        calculateBoneModelSpace(boneIndex);
    }
    m_boneFlags[boneIndex] = BONE_FLAG_MODEL_WRITTEN;
    unk29 = false;
    return &m_modelPose[boneIndex];
}

hkArray<hkQsTransform>& hkPose::accessPoseModelSpace() {
    syncModelSpace();
    return writeAccessPoseModelSpace();
}

hkArray<hkQsTransform>& hkPose::writeAccessPoseModelSpace() {
    int n = m_skeleton->m_numBones;
    for (int i = 0; i < n; i++) {
        m_boneFlags[i] = BONE_FLAG_MODEL_WRITTEN;
    }
    unk29 = false;
    m_modelPoseValid = true;
    return m_modelPose;
}

void hkPose::setToReferencePose() {
    hkSkeleton* skel = m_skeleton;
    hkString::memCpy(m_localPose.m_data, skel->m_referencePose, skel->m_numBones * sizeof(hkQsTransform));
    for (int i = 0; i < m_skeleton->m_numBones; i++) {
        m_boneFlags[i] = BONE_FLAG_LOCAL_DIRTY;
    }
    m_modelPoseValid = false;
    unk29 = true;
}
