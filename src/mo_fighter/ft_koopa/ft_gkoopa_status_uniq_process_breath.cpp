#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_CTOR_NOINLINE // MATCH-ONLY: the original builds the clamped speed pair through the out-of-line constructor
#define MT_VEC2F_ASSIGN_NOINLINE // MATCH-ONLY: ... and assigns it through the out-of-line operator=
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <math.h>
#include <so/camera/so_camera_module_simple.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the end-of-breath helpers of Giga Bowser (defined after the vtable members in the original object): the first
// clears the update flag (0x80) of the four kinetic energies, the second resets the work state of the breath and, in the air,
// restarts the gravity and controller energies from the Giga Bowser extend parameters (index 2).
extern "C" void fn_102_156C0(soModuleAccesser* moduleAccesser);
extern "C" void fn_102_15788(soModuleAccesser* moduleAccesser);

// HYPOTHESIS: Giga Bowser's fire breath: the same work layout as Bowser's (frame counter 0x20000000, phase 0x20000002 and
// 0x20000003, rumble cycle 0x20000004, sound handles 0x20000006/7, head angle float 0x21000004, charge 0x11000013).
class ftGKoopaStatusUniqProcessBreath : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessBreath() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftGKoopaStatusUniqProcessBreath::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getSituationModule().getKind();
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000005);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000006);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000007);
}

void ftGKoopaStatusUniqProcessBreath::execStatus(soModuleAccesser* moduleAccesser) {
    int phase = moduleAccesser->getWorkManageModule().getInt(0x20000002);
    if (phase != moduleAccesser->getWorkManageModule().getInt(0x20000003)) {
        moduleAccesser->getWorkManageModule().setInt(phase, 0x20000003);
        if (phase == 2) {
            moduleAccesser->getSoundModule().stopStatusSE();
        }
    }
    switch (phase) {
    case 1: {
        float low = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
        float high = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0);
        float ratio = (moduleAccesser->getWorkManageModule().getFloat(0x11000013) - low) / (high - low);
        int frame = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        if (frame >= 3 && moduleAccesser->getWorkManageModule().getInt(0x20000006) == -1) {
            int handle = moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(0x1a20), false, 0);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000006);
        } else if (frame >= 11 && moduleAccesser->getWorkManageModule().getInt(0x20000007) == -1) {
            int handle = moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(0x1a21), false, 0);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000007);
        }
        int cycle = moduleAccesser->getWorkManageModule().getInt(0x20000004);
        if (cycle == 0) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000013);
            int handle = moduleAccesser->getWorkManageModule().getInt(0x20000007);
            if (handle != -1) {
                float volume = ratio;
                float minVolume = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb0, 0);
                if (ratio < minVolume) {
                    volume = minVolume;
                }
                moduleAccesser->getSoundModule().setSEVol(volume, handle, 0);
            }
            soControllerModule& controller = moduleAccesser->getControllerModule();
            int rumbleKind = 1;
            if (ratio > 0.5f) {
                rumbleKind = 0;
            }
            controller.setRumble(rumbleKind, 0, true, -1);
        }
        int next = cycle + 1;
        if (next >= soValueAccesser::getConstantInt(moduleAccesser, 0x5dc5, 0)) {
            next = 0;
        }
        moduleAccesser->getWorkManageModule().setInt(next, 0x20000004);
        if (ratio > 0.5f) {
            if (frame % soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0) == 0) {
                moduleAccesser->getCameraModule().reqQuake(static_cast<soCameraModule::QuakeKind>(0), moduleAccesser);
            }
        }
        moduleAccesser->getWorkManageModule().setInt(frame + 1, 0x20000000);
        float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        float stickY = moduleAccesser->getControllerModule().getStickY();
        double stickMagnitude = fabs(stickY); // MATCH-ONLY: narrowed to float before the compare
        if ((float)stickMagnitude > 0.5f) {
            float maxAngle = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0);
            float minAngle = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0);
            float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
            angle += rate * stickY;
            register float limitHigh = maxAngle;
            register float value = angle;
            register float limitLow = -minAngle;
            register float fromLower = value - limitLow;
            register float bounded;
            // MATCH-ONLY: the fighter units are built without inlining of the nw4r FSelect asm helper.
            asm { fsel bounded, fromLower, value, limitLow }
            register float fromUpper = bounded - limitHigh;
            register float clamped;
            asm { fsel clamped, fromUpper, limitHigh, bounded }
            angle = clamped;
            moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
        }
        int headNode = moduleAccesser->getModelModule().getNodeId("HeadN");
        float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0);
        Vec3f rot;
        rot.m_x = -angle * scale;
        rot.m_y = 0.0f;
        rot.m_z = 0.0f;
        moduleAccesser->getModelModule().setNodeRotate(headNode, &rot);
        break;
    }
    }
}

void ftGKoopaStatusUniqProcessBreath::exitStatus(soModuleAccesser* moduleAccesser, int) { }

ftGKoopaStatusUniqProcessBreath g_ftGKoopaStatusUniqProcessBreath;

extern "C" void fn_102_156C0(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(1)->disable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

extern "C" void fn_102_15788(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int situation = moduleAccesser->getSituationModule().getKind();
    work.clearAll(2);
    work.offFlag(0x22000010);
    work.offFlag(0x22000011);
    work.offFlag(0x22000012);
    work.offFlag(0x22000013);
    work.offFlag(0x22000014);
    work.onFlag(0x22000015);
    work.setInt(0, 0x20000000);
    work.offFlag(0x1200003f);
    if (situation == 2) {
        float* param = reinterpret_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_GKoopa)->extendParam[2]);
        Vec2f speed;
        Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        Vec2f airSpeed;
        Vec2f::copy(airSpeed, speed);
        airSpeed.m_y = param[1];
        fn_102_156C0(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, airSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = -param[2];
        gravity.m_fallSpeedMax = param[3];
        gravity.unk1C = param[3];
        gravity.enable();
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(airSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param[7];
        controller.m_speedTarget = Vec2f(param[5], controller.m_speedTarget.m_y);
        controller.m_speedLimit = Vec2f(param[5], controller.m_speedLimit.m_y);
        controller.enable();
    }
}
