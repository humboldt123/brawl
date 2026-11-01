#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// BYTES: keep native out-of-line Vec3f construction for resetEnergy inputs.
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftDededeStatusUniqProcessSpecialSuperJumpJump : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJumpJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
};

void ftDededeStatusUniqProcessSpecialSuperJumpJump::initStatus(soModuleAccesser* moduleAccesser) {
    float stickX = moduleAccesser->getControllerModule().getStickX();
    float horizontalSpeed = stickX * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb7, 0);
    float verticalSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb8, 0);
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        verticalSpeed *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
    }
    stickX = __fabsf(stickX);
    verticalSpeed -= stickX * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb9, 0);

    ftKineticEnergyGravity& vertical = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f verticalInitial(0.0f, verticalSpeed);
    vertical.resetEnergy(0, &verticalInitial, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    vertical.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0);
    vertical.enable();

    ftKineticEnergyStop& horizontal = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f horizontalInitial(horizontalSpeed, 0.0f);
    horizontal.resetEnergy(0x16, &horizontalInitial, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    horizontal.m_accel = Vec2f(0.0f, 0.0f);
    horizontal.m_speedTarget = Vec2f(horizontalSpeed, 0.0f);
    horizontal.m_brake = Vec2f(0.0f, 0.0f);
    horizontal.m_speedLimit = Vec2f(horizontalSpeed, 0.0f);
    horizontal.enable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();

    moduleAccesser->getWorkManageModule().onFlag(0x22000011);
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f original = moduleAccesser->getModelModule().getNodeRotate(7);
        // MATCH-ONLY: retain the returned Z load before assembling the native rotation copy.
        float z = original.m_z;
        Vec3f rotate;
        rotate.m_x = original.m_x;
        rotate.m_y = 180.0f;
        rotate.m_z = z;
        moduleAccesser->getModelModule().setNodeRotate(7, &rotate);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x22000015);
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000001);
        if (moduleAccesser->getWorkManageModule().getInt(0x20000001) > soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0)) {
            Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
            ftKineticEnergyGravity& vertical = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            Vec2f speed;
            speed.m_x = 0.0f;
            speed.m_y = sumSpeed.m_y;
            vertical.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            vertical.enable();
            moduleAccesser->getWorkManageModule().offFlag(0x22000011);
            moduleAccesser->getWorkManageModule().onFlag(0x22000015);
        }
    }
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f original = moduleAccesser->getModelModule().getNodeRotate(7);
        // MATCH-ONLY: retain the returned Z load before assembling the native rotation copy.
        float z = original.m_z;
        Vec3f rotate;
        rotate.m_x = original.m_x;
        rotate.m_y = 180.0f;
        rotate.m_z = z;
        moduleAccesser->getModelModule().setNodeRotate(7, &rotate);
    }
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::execStop(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f original = moduleAccesser->getModelModule().getNodeRotate(7);
        // MATCH-ONLY: keep the original Z load live while building the replacement rotation.
        float z = original.m_z;
        Vec3f rotate;
        rotate.m_x = original.m_x;
        rotate.m_y = 180.0f;
        rotate.m_z = z;
        moduleAccesser->getModelModule().setNodeRotate(7, &rotate);
    }
}

void ftDededeStatusUniqProcessSpecialSuperJumpJump::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x12b || status == 0x12e || status == 0x12c) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        if (work.isFlag(0x22000010)) {
            moduleAccesser->getPostureModule().setLr(-1.0f);
        } else {
            moduleAccesser->getPostureModule().setLr(1.0f);
        }
    }
    moduleAccesser->getModelModule().clearNodeSRT(7);
}

ftDededeStatusUniqProcessSpecialSuperJumpJump g_ftDededeStatusUniqProcessSpecialSuperJumpJump;