#pragma once

#include <havok/hkShape.h>

// Summary of the collision spheres a shape reports: count and a flag (HYPOTHESIS: names and meaning).
struct hkCollisionSpheresInfo {
    int m_numSpheres;  // 0x00
    u8 m_flag;         // 0x04
};

// Shape that can be represented by collision spheres (0x0C bytes, no members). Layout from
// hkSphereRepShapeClass.cpp. Adds two vtable slots after hkShape: 0x28 getCollisionSpheresInfo, 0x2C getCollisionSpheres.
struct hkSphereRepShape : hkShape {
    virtual void getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const; // 0x28
    virtual hkVector4* getCollisionSpheres(hkVector4* out) const;            // 0x2C
};
