// Local shadow of BrawlHeaders/so/ground/so_ground_util.h: adds the ray check entry points.
#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <mt/mt_vector.h>

class soModuleAccesser;

namespace soGroundUtil {
    enum DeadAreaCheckResult {
        Dead_Area_Check_Result_None = -1,
        Dead_Area_Check_Result_Outside_Up = 0,
        Dead_Area_Check_Result_Outside_Down = 1,
        Dead_Area_Check_Result_Outside_Left = 2,
        Dead_Area_Check_Result_Outside_Right = 3,
    };

    DeadAreaCheckResult checkDeadArea(Vec3f* pos, float, float);

    // Local shadow additions (HYPOTHESIS signatures, taken from the soGroundModuleImpl::rayCheck forwarders).
    bool rayCheck(soModuleAccesser* moduleAccesser, Vec3f* from, Vec3f* to, bool unk);
    bool rayCheck(soModuleAccesser* moduleAccesser, Vec3f* from, Vec3f* to, Vec3f* outPos, Vec3f* outNormal, bool unk);
    bool rayCheck(soModuleAccesser* moduleAccesser, Vec3f* from, Vec3f* to, int* outLineId, Vec3f* outPos, Vec3f* outNormal, bool unk);
}