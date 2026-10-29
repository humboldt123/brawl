#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <types.h>

class cmCameraParam {
public:
    // HYPOTHESIS: each value is followed by the value it moves towards; resetInternalCameraParam copies one into the other.
    Vec3f m_pos;
    Vec3f m_targetPos;
    Vec3f m_eye;
    Vec3f m_targetEye;
    float m_fovy;
    float m_targetFovy;
    bool unk38;

    void clear();
    // HYPOTHESIS: the parameter names are inferred from the blend towards the new field of view.
    void updateFovyNormal(float fovy, float rate);
};
static_assert(sizeof(cmCameraParam) == 0x3C, "Class is wrong size!");
