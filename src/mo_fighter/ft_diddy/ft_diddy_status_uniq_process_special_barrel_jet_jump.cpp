// MATCH-ONLY: keep native out-of-line Vec3f construction for resetEnergy inputs.
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the weak ABS<f> copy that the module map places in the pop-gun unit (still unnamed there).
extern "C" float fn_115_E59C(float);
float vec2fAngle(Vec2f*, Vec2f*);

// HYPOTHESIS: the fighter attribute block is copied whole (0x2E4 bytes); the four floats at 0x64 seed the air gravity.
struct ftDiddyAirParam {
    u8 unk0[0x64];
    float unk64;
    float unk68;
    float unk6C;
    float unk70;
    u8 unk74[0x270];
};

// HYPOTHESIS: a file-scope rotation table; only the first entry (the per-frame lean step) is referenced.
static const float s_rotationParam[4] = { 12.0f, 12.0f, 24.0f, 12.0f };

class ftDiddyStatusUniqProcessSpecialBarrelJetJump : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJetJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    float setBodyRotationMoveDir(soModuleAccesser* moduleAccesser, float maxStep);
    // HYPOTHESIS: the node id comes before the angle.
    void setNodeRotation(soModuleAccesser* moduleAccesser, int nodeId, float rotation);
};

void ftDiddyStatusUniqProcessSpecialBarrelJetJump::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x127: {
        float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb0, 0);
        float lr = moduleAccesser->getPostureModule().getLr();
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            speed *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfb2, 0);
        }
        speed += moduleAccesser->getWorkManageModule().getInt(0x20000000) * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb1, 0);
        float angle = 0.017453292f * moduleAccesser->getWorkManageModule().getFloat(0x21000005);
        Vec2f velocity(lr * (speed * cosf(angle)), speed * sinf(angle));
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setFloat(work.getFloat(0x21000005), 0x21000006);

        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec2f gravitySpeed(0.0f, velocity.m_y);
        gravity.resetEnergy(0, &gravitySpeed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        ftDiddyAirParam param = *reinterpret_cast<ftDiddyAirParam*>(g_ftCommonDataAccesser.getParam(Fighter_Diddy));
        gravity.m_gravity = -param.unk64;
        gravity.m_fallSpeedMax = param.unk68;
        gravity.unk18 = param.unk6C;
        gravity.unk1C = param.unk70;

        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        Vec2f stopSpeed(velocity.m_x, 0.0f);
        stop.resetEnergy(6, &stopSpeed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        float rotation = setBodyRotationMoveDir(moduleAccesser, s_rotationParam[0]);
        moduleAccesser->getWorkManageModule().setFloat(rotation, 0x2100000d);
        break;
    }
    case 0x12a: {
        float rotation = moduleAccesser->getWorkManageModule().getFloat(0x2100000d);
        setNodeRotation(moduleAccesser, 5, rotation);
        if (0.0f == rotation) {
            rotation = 360.0f;
        }
        float spin = 24.0f;
        if (rotation < 0.0f) {
            spin = -spin;
        }
        moduleAccesser->getWorkManageModule().setFloat(rotation, 0x2100000d);
        moduleAccesser->getWorkManageModule().setFloat(spin, 0x2100000e);
        break;
    }
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJetJump::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x127:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000015)) {
            float rotation = setBodyRotationMoveDir(moduleAccesser, s_rotationParam[0]);
            moduleAccesser->getWorkManageModule().setFloat(rotation, 0x2100000d);
        } else {
            setNodeRotation(moduleAccesser, 5, moduleAccesser->getWorkManageModule().getFloat(0x2100000d));
        }
        break;
    case 0x12a: {
        float rotation = moduleAccesser->getWorkManageModule().getFloat(0x2100000d);
        float spin = moduleAccesser->getWorkManageModule().getFloat(0x2100000e);
        rotation += spin;
        if (fn_115_E59C(rotation) > 360.0f) {
            rotation = 360.0f * (spin < 0.0f ? -1 : 1);
        }
        setNodeRotation(moduleAccesser, 5, rotation);
        moduleAccesser->getWorkManageModule().setFloat(rotation, 0x2100000d);
        break;
    }
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJetJump::execFixPos(soModuleAccesser* moduleAccesser) {
    Vec2f speed;
    Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x127:
        if (moduleAccesser->getGroundModule().isTouch(grCollStatus::TouchMask(0xff), 0)) {
            Vec2f normal;
            Vec2f::copy(normal, moduleAccesser->getGroundModule().getTouchNormal(0xff, 0));
            float angle = vec2fAngle(&speed, &normal);
            if (angle - 1.5707964f >= 0.017453292f * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb5, 0)) {
                float bounce = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb8, 0);
                moduleAccesser->getWorkManageModule().setFloat(normal.m_x * bounce, 0x21000007);
                moduleAccesser->getWorkManageModule().setFloat(normal.m_y * bounce, 0x21000008);
                moduleAccesser->getStatusModule().changeStatusRequest(0x129, moduleAccesser);
            }
        }
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000014) && speed.m_y < 0.0f) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000014);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000016)) {
            if (fn_115_E59C(moduleAccesser->getWorkManageModule().getFloat(0x2100000d)) >= 12.0f) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x12a, moduleAccesser);
            }
        }
        break;
    case 0x12a:
        if (360.0f == fn_115_E59C(moduleAccesser->getWorkManageModule().getFloat(0x2100000d))) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x10, moduleAccesser);
        }
        break;
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJetJump::execStop(soModuleAccesser* moduleAccesser) {
    setNodeRotation(moduleAccesser, 5, moduleAccesser->getWorkManageModule().getFloat(0x2100000d));
}

void ftDiddyStatusUniqProcessSpecialBarrelJetJump::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x128:
    case 0x129:
        return;
    default:
        moduleAccesser->getGenerateArticleManageModule().removeExist(1, 0);
        moduleAccesser->getEffectModule().killKind(0x1c0002, true, true);
        break;
    }
}

float ftDiddyStatusUniqProcessSpecialBarrelJetJump::setBodyRotationMoveDir(soModuleAccesser* moduleAccesser, float maxStep) {
    Vec2f dir;
    Vec2f::copy(dir, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    dir.normalize();
    float lr = moduleAccesser->getPostureModule().getLr();
    float target = lr * (57.29578f * atan2f(dir.m_x, dir.m_y));
    float current = moduleAccesser->getWorkManageModule().getFloat(0x2100000d);
    float absTarget = fn_115_E59C(target);
    float diff = fn_115_E59C(current) - absTarget;
    if (0.0f == current && 180.0f == fn_115_E59C(diff)) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000018);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000018)) {
        target = fn_115_E59C(target);
    }
    if (fn_115_E59C(diff) > maxStep) {
        target = current + maxStep * (target < 0.0f ? -1 : 1);
    }
    Vec3f rotation = moduleAccesser->getModelModule().getNodeRotate(5);
    rotation.m_x = target;
    moduleAccesser->getModelModule().setNodeRotate(5, &rotation);
    return rotation.m_x;
}

void ftDiddyStatusUniqProcessSpecialBarrelJetJump::setNodeRotation(soModuleAccesser* moduleAccesser, int nodeId, float rotation) {
    Vec3f nodeRotation = moduleAccesser->getModelModule().getNodeRotate(nodeId);
    nodeRotation.m_x = rotation;
    moduleAccesser->getModelModule().setNodeRotate(nodeId, &nodeRotation);
}

ftDiddyStatusUniqProcessSpecialBarrelJetJump g_ftDiddyStatusUniqProcessSpecialBarrelJetJump;
