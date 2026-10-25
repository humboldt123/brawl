#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <math.h>
#include <nw4r/math/math_arithmetic.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: Bowser's fire breath (status kinds around Special N): work int 0x20000000 counts the frames of the breath, 0x20000002
// holds the phase (1 = breathing, 2 = ending), 0x20000003 the phase of the previous frame, 0x20000004 the position in the rumble
// cycle, 0x20000006/7 the two sound ids and 0x20000008/9 their handles; work float 0x21000004 is the head angle that the stick
// moves, 0x11000013 the charge. Sub kind 5 is Kirby with the copied breath.
// HYPOTHESIS: inline clamp helper (fsel based), as in wn_lucario_aura_ball.cpp.
static inline float clampf(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

class ftKoopaStatusUniqProcessBreath : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessBreath() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftKoopaStatusUniqProcessBreath g_ftKoopaStatusUniqProcessBreath;

void ftKoopaStatusUniqProcessBreath::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000005);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000008);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000009);
    if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
        moduleAccesser->getWorkManageModule().setInt(0x1ad7, 0x20000006);
        moduleAccesser->getWorkManageModule().setInt(0x1ad8, 0x20000007);
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000a);
    } else {
        moduleAccesser->getWorkManageModule().setInt(0xefd, 0x20000006);
        moduleAccesser->getWorkManageModule().setInt(0xefe, 0x20000007);
        int headNode = moduleAccesser->getModelModule().getNodeId("HeadN");
        moduleAccesser->getWorkManageModule().setInt(headNode, 0x2000000a);
    }
}

void ftKoopaStatusUniqProcessBreath::exitStatus(soModuleAccesser* moduleAccesser, int) { }

void ftKoopaStatusUniqProcessBreath::execStatus(soModuleAccesser* moduleAccesser) {
    int phase = moduleAccesser->getWorkManageModule().getInt(0x20000002);
    if (phase != moduleAccesser->getWorkManageModule().getInt(0x20000003)) {
        moduleAccesser->getWorkManageModule().setInt(phase, 0x20000003);
        if (phase == 2) {
            moduleAccesser->getSoundModule().stopStatusSE();
        }
    }
    switch (phase) {
    case 1: {
        float ratio;
        float low = moduleAccesser->getConstantFloatKirby(0xfa9);
        float high = moduleAccesser->getConstantFloatKirby(0xfa8);
        ratio = (moduleAccesser->getWorkManageModule().getFloat(0x11000013) - low) / (high - low);
        int frame = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        if (frame >= 3 && moduleAccesser->getWorkManageModule().getInt(0x20000008) == -1) {
            int soundId = moduleAccesser->getWorkManageModule().getInt(0x20000006);
            int handle = moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(soundId), false, 0);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000008);
        } else if (frame >= 11 && moduleAccesser->getWorkManageModule().getInt(0x20000009) == -1) {
            int soundId = moduleAccesser->getWorkManageModule().getInt(0x20000007);
            int handle = moduleAccesser->getSoundModule().playStatusSE(static_cast<SndID>(soundId), false, 0);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000009);
        }
        int cycle = moduleAccesser->getWorkManageModule().getInt(0x20000004);
        if (cycle == 0) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000013);
            int handle = moduleAccesser->getWorkManageModule().getInt(0x20000009);
            if (handle != -1) {
                float volume = ratio;
                float minVolume = moduleAccesser->getConstantFloatKirby(0xfb0);
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
        if (next >= moduleAccesser->getConstantIntKirby(0x5dc2)) {
            next = 0;
        }
        moduleAccesser->getWorkManageModule().setInt(next, 0x20000004);
        moduleAccesser->getWorkManageModule().setInt(frame + 1, 0x20000000);
        float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        float stickY = moduleAccesser->getControllerModule().getStickY();
        double stickMagnitude = fabs(stickY); // MATCH-ONLY: narrowed to float before the compare
        if ((float)stickMagnitude > 0.5f) {
            float maxAngle = moduleAccesser->getConstantFloatKirby(0xfa2);
            float minAngle = moduleAccesser->getConstantFloatKirby(0xfa3);
            float rate = moduleAccesser->getConstantFloatKirby(0xfa4);
            angle += rate * stickY;
            angle = clampf(angle, -minAngle, maxAngle);
            moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
        }
        int headNode = moduleAccesser->getWorkManageModule().getInt(0x2000000a);
        if (headNode != 0) {
            float scale = moduleAccesser->getConstantFloatKirby(0xfa5);
            float negated = -angle;
            Vec3f rot;
            rot.m_y = 0.0f;
            rot.m_x = negated * scale;
            rot.m_z = 0.0f;
            moduleAccesser->getModelModule().setNodeRotate(headNode, &rot);
        }
        break;
    }
    }
}
