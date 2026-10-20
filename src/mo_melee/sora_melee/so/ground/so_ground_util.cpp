#include <so/ground/so_ground_util.h>

#include <cm/cm_camera_controller.h>
#include <math.h>
#include <so/so_module_accesser.h>
#include <st/st_utility.h>

// HYPOTHESIS: unk158/unk15C/unk160/unk164 are the camera controller's left/right/top/bottom dead zone edges.
namespace soGroundUtil {

// Which side of the camera's dead zone `pos` (with a `margin` around it) has left, or Dead_Area_Check_Result_None.
// For the top edge, a non-zero `rate` shrinks the dead zone horizontally (the corners count as left/right instead of up).
DeadAreaCheckResult checkDeadArea(Vec3f* pos, float margin, float rate) {
    CameraController* camera = CameraController::getInstance();
    if (pos->m_x - margin < camera->unk158) {
        return Dead_Area_Check_Result_Outside_Left;
    }
    if (pos->m_x + margin > camera->unk15C) {
        return Dead_Area_Check_Result_Outside_Right;
    }
    if (pos->m_y + margin > camera->unk160) {
        if (rate != 0.0f) {
            float inset = rate * (camera->unk15C - camera->unk158);
            if (pos->m_x < camera->unk158 + inset) {
                return Dead_Area_Check_Result_Outside_Left;
            }
            if (pos->m_x > camera->unk15C - inset) {
                return Dead_Area_Check_Result_Outside_Right;
            }
        }
        return Dead_Area_Check_Result_Outside_Up;
    }
    if (pos->m_y - margin < camera->unk164) {
        return Dead_Area_Check_Result_Outside_Down;
    }
    return Dead_Area_Check_Result_None;
}

// How far along the dead zone border `pos` is (1.0 while inside).
float checkDeadDivisionRate(Vec3f* pos) {
    CameraController* camera = CameraController::getInstance();
    if (pos->m_x < camera->unk158 || pos->m_x > camera->unk15C) {
        float height = camera->unk160 - camera->unk164;
        float length = fabs(height);
        return (pos->m_y - camera->unk164) / length;
    }
    if (pos->m_y > camera->unk160 || pos->m_y < camera->unk164) {
        float width = camera->unk15C - camera->unk158;
        float length = fabs(width);
        return (camera->unk15C - pos->m_x) / length;
    }
    return 1.0f;
}

// HYPOTHESIS: slot 0x28 of the module at enumeration + 0x68 fills in a value that the ray checks below never read.
class RayCheckHookModule {
public:
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void hook(void* out);
};

static inline void callRayCheckHook(soModuleAccesser* moduleAccesser, void* out) {
    ((RayCheckHookModule*)moduleAccesser->m_enumerationStart->m_debugModule)->hook(out);
}

bool rayCheck(soModuleAccesser* moduleAccesser, Vec3f* from, Vec3f* to, bool unk) {
    int hook[2];
    callRayCheckHook(moduleAccesser, hook);
    return stRayCheck(from, to, unk, NULL, false, 1);
}

bool rayCheck(soModuleAccesser* moduleAccesser, Vec3f* from, Vec3f* to, Vec3f* outPos, Vec3f* outNormal, bool unk) {
    int hook[2];
    callRayCheckHook(moduleAccesser, hook);
    return stRayCheck(from, to, outPos, outNormal, unk, NULL, false, 1);
}

bool rayCheck(soModuleAccesser* moduleAccesser, Vec3f* from, Vec3f* to, int* outLineId, Vec3f* outPos, Vec3f* outNormal, bool unk) {
    int hook[2];
    callRayCheckHook(moduleAccesser, hook);
    return stRayCheck(from, to, outLineId, outPos, outNormal, unk, false, 1);
}

}
