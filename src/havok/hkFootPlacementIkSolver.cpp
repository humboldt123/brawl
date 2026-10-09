// Havok translation unit hkFootPlacementIkSolver.o (main.dol 0x8032D764-0x8032DA40).
// Partly decompiled (getBoneModelSpace). Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8032D764   520  setRotatedDir   [map: hkVector4__setRotatedDir1]
//   0x8032D96C     8  getRotation   [map: hkQsTransform__getRotation]
//   0x8032D974     4  getTranslation   [map: hkQsTransform__getTranslation]
//   0x8032D978    36  getBoneModelSpace   [map: hkPose__getBoneModelSpace]
//   0x8032D99C   164  normalize   [map: hkQuaternion__normalize]

#include <havok/hkPose.h>

hkQsTransform* hkPose::getBoneModelSpace(int boneIndex) {
    if (m_boneFlags[boneIndex] & BONE_FLAG_LOCAL_DIRTY) {
        return calculateBoneModelSpace(boneIndex);
    }
    return &m_modelPose[boneIndex];
}
