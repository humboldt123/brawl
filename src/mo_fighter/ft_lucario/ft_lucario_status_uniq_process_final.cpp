#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// BYTES: the Aura Storm statuses call the out-of-line Vec3f constructor and assignment, and this unit owns the weak Vec3f scale.
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_MUL_INLINE
#include <cm/cm_camera_controller.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <st/stage.h>
#include <types.h>
#include <math.h>

// HYPOTHESIS: clampf(value, min, max) of ft_lucario.o per the map; it stays fn_ in the ft_lucario.cpp unit.
extern "C" float fn_118_8F8C(float value, float min, float max);

void rotate(soModuleAccesser* moduleAccesser);
void camera(soModuleAccesser* moduleAccesser);

class ftLucarioStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessFinalEntry : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalEntry() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessFinalHold : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalHold() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    static void set_rot_angle(soModuleAccesser* moduleAccesser);
};

class ftLucarioStatusUniqProcessFinalAttack : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalAttack() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessFinalEnd() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

void ftLucarioStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
}

void ftLucarioStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000000) == 1) {
        soPostureModule& posture = moduleAccesser->getPostureModule();
        Vec3f pos;
        pos = posture.getPos();
        pos.m_y += soValueAccesser::getConstantFloat(moduleAccesser, 0xfca, 0);
        posture.setPos(&pos);
    }
}

void ftLucarioStatusUniqProcessFinalEntry::initStatus(soModuleAccesser* moduleAccesser) { camera(moduleAccesser); }
void ftLucarioStatusUniqProcessFinalEntry::execStatus(soModuleAccesser* moduleAccesser) {
    soMotionModule& motion = moduleAccesser->getMotionModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Vec3f kirifudaPos;
    g_Stage->getKirifudaPos(&kirifudaPos, 0);
    Vec3f startPos;
    startPos = kirifudaPos;
    startPos.m_y = 100.0f + CameraController::getInstance()->m_stageCameraBounds.m_up;
    float frame = motion.getFrame();
    float t = sin(1.5707964f * (frame / motion.getEndFrame()));
    float rest = 1.0f - t;
    Vec3f pos;
    pos.m_x = kirifudaPos.m_x * t + startPos.m_x * rest;
    pos.m_y = kirifudaPos.m_y * t + startPos.m_y * rest;
    pos.m_z = kirifudaPos.m_z * t + startPos.m_z * rest;
    posture.setPos(&pos);
}

void ftLucarioStatusUniqProcessFinalHold::initStatus(soModuleAccesser* moduleAccesser) { camera(moduleAccesser); }
void ftLucarioStatusUniqProcessFinalHold::execStatus(soModuleAccesser* moduleAccesser) { rotate(moduleAccesser); }

void ftLucarioStatusUniqProcessFinalAttack::initStatus(soModuleAccesser* moduleAccesser) {
    camera(moduleAccesser);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000001);
}
void ftLucarioStatusUniqProcessFinalAttack::execStatus(soModuleAccesser* moduleAccesser) {
    rotate(moduleAccesser);
    soCollisionAttackModule& collisionAttack = moduleAccesser->getCollisionAttackModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (collisionAttack.isAttack(0, 0)) {
        work.decInt(0x20000001);
        if (work.getInt(0x20000001) <= 0) {
            soGroundModule& ground = moduleAccesser->getGroundModule();
            soEffectModule& effect = moduleAccesser->getEffectModule();
            soModelModule& model = moduleAccesser->getModelModule();
            float z = ground.getZ();
            Vec3f start;
            start = model.getNodeGlobalPosition(model.getCorrectNodeId(0x12e), true);
            start.m_z = z;
            float angle = work.getFloat(0x21000004) - 90.0f;
            angle = 0.017453292f * angle;
            Vec3f dir;
            float c = cos(angle);
            dir.m_x = -1.0f * c * moduleAccesser->getPostureModule().getLr();
            dir.m_y = sin(angle);
            dir.m_z = 0.0f;
            Vec3f ray;
            ray = dir * 500.0f;
            for (int i = 0; i < 5; i++) {
                Vec3f hitPos;
                Vec3f hitNormal;
                if (!ground.rayCheck(&start, &ray, &hitPos, &hitNormal, true)) {
                    break;
                }
                hitPos.m_z = z;
                effect.req(static_cast<EfID>(0xfc0003), &hitPos, &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
                start = hitPos + dir * 5.0f;
            }
            work.setInt(10, 0x20000001);
        }
    }
}

void ftLucarioStatusUniqProcessFinalEnd::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float angle = work.getFloat(0x21000004);
    float rate = -(angle / moduleAccesser->getMotionModule().getEndFrame(0x1ea));
    work.setFloat(rate, 0x21000005);
}

void ftLucarioStatusUniqProcessFinalEnd::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float angle = work.getFloat(0x21000004);
    float rate = work.getFloat(0x21000005);
    float next = angle + rate;
    work.setFloat(next, 0x21000004);
}

void rotate(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    float stickMin = soValueAccesser::getConstantFloat(moduleAccesser, 0xc2d, 0);
    float stickMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xc36, 0);
    float stickX = controller.getStickX();
    float dir;
    if (stickX >= 0.0f) {
        dir = 1.0f;
    } else {
        dir = -1.0f;
        double wide = fabs(stickX);
        stickX = wide;
    }
    dir *= fn_118_8F8C((stickX - stickMin) / (stickMax - stickMin), 0.0f, 1.0f);
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfcc, 0);
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xfcb, 0);
    float angle = work.getFloat(0x21000004);
    work.setFloat(fn_118_8F8C(angle + speed * dir * (-1.0f * posture.getLr()), -limit, limit), 0x21000004);
    if (0.0f != dir && work.getInt(0x20000001) > 3) {
        work.setInt(3, 0x20000001);
    }
}

void ftLucarioStatusUniqProcessFinalHold::set_rot_angle(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soModelModule& model = moduleAccesser->getModelModule();
    float angle = work.getFloat(0x21000004);
    int node = model.getCorrectNodeId(0x12e);
    model.setNodeRotate(node, &Vec3f(angle, 0.0f, 0.0f));
}

void camera(soModuleAccesser* moduleAccesser) {
    soCameraModule& cameraModule = moduleAccesser->getCameraModule();
    // HYPOTHESIS: the result of this StageObject query is unused in the original.
    moduleAccesser->getStageObject().soGetSubKind();
    CameraController* cameraController = CameraController::getInstance();
    soCameraRange* range = cameraModule.getCameraRangeData(0);
    Stage* stage = g_Stage;
    Vec3f pos;
    stage->getKirifudaPos(&pos, 0);
    pos.m_y += range->unk0;
    Rect2D rect;
    rect.m_up = cameraController->m_stageCameraBounds.m_up - (pos.m_y + range->m_range.m_up);
    rect.m_down = cameraController->m_stageCameraBounds.m_down - (pos.m_y + range->m_range.m_down);
    if (moduleAccesser->getPostureModule().getLr() >= 0.0f) {
        rect.m_right = cameraController->m_stageCameraBounds.m_right - (pos.m_x + range->m_range.m_left);
        rect.m_left = cameraController->m_stageCameraBounds.m_left - (pos.m_x + range->m_range.m_right);
    } else {
        rect.m_right = -(cameraController->m_stageCameraBounds.m_left - (pos.m_x - range->m_range.m_left));
        rect.m_left = -(cameraController->m_stageCameraBounds.m_right - (pos.m_x - range->m_range.m_right));
    }
    cameraModule.addCameraRangeRect(&rect, 0);
    float height = cameraController->m_stageCameraBounds.m_up - cameraController->m_stageCameraBounds.m_down;
    float ratio = -0.3f;
    switch (stage->m_stageKind) {
    case 0x31:
    case 0x35:
        ratio = 0.0f;
        break;
    }
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = height * ratio;
    offset.m_z = 0.0f;
    cameraModule.addCameraRangeOffset(&offset, 0);
}

ftLucarioStatusUniqProcessFinal g_ftLucarioStatusUniqProcessFinal;
ftLucarioStatusUniqProcessFinalEntry g_ftLucarioStatusUniqProcessFinalEntry;
ftLucarioStatusUniqProcessFinalHold g_ftLucarioStatusUniqProcessFinalHold;
ftLucarioStatusUniqProcessFinalAttack g_ftLucarioStatusUniqProcessFinalAttack;
ftLucarioStatusUniqProcessFinalEnd g_ftLucarioStatusUniqProcessFinalEnd;
