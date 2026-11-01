#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes (same as the shoot scope unit).
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_log_transactor.h>
#include <ft/ft_status_uniq_process_hammer.h>
#include <ft/ft_util.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessHammer g_ftStatusUniqProcessHammer;

// Walk speed helper (ftStatusUniqProcessWalkUtil::calcWalkSpeed), declared by its map address.
extern "C" void fn_27_165458(soModuleAccesser* moduleAccesser, int speedWork, float current, float target, float acceleration,
                             float deceleration, float speedLimit, float speedScale);

// Local views for module slots without a declared interface.
class ftHammerVisibilityView {
public:
    virtual ~ftHammerVisibilityView() { }
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void slot18();
};

class ftHammerEnergyView {
public:
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void slot20();
};

class ftHammerMotionView {
public:
    virtual ~ftHammerMotionView() { }
};

class ftHammerItemView {
public:
    virtual void pad0();
};

// Hammer work variables (HYPOTHESIS): 0x20000001 swing frames left, 0x20000002 hold frames left, 0x20000003 / 0x20000004 effect
// counters, 0x21000004 walk speed, 0x1200000C hammer finished flag, 0x1200000D hold exhausted flag, 0x22000011 hammer
// gold-hit flag, 0x1000001B / 0x1000001D hammer state ints.
void ftStatusUniqProcessHammer::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xE0: {
        Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        moduleAccesser->getWorkManageModule().setFloat(sumSpeed.m_x, 0x21000004);
        break;
    }
    }
    if (!ftUtil::isHammerStatus(moduleAccesser, moduleAccesser->getStatusModule().getPrevStatusKind(0))) {
        moduleAccesser->getMotionModule().removePartialAnimChr(0);
        moduleAccesser->getMotionModule().enablePartialCancelNode(false);
        moduleAccesser->getItemManageModule().setHaveItemHoldAnim(false, 0);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000002);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000001);
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        reinterpret_cast<ftHammerVisibilityView*>(moduleAccesser->m_enumerationStart->m_visibilityModule)->slot18();
        if (moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0x1D) {
            moduleAccesser->getEffectModule().reqCommon(0x14, 0.0f);
            int swingFrames = moduleAccesser->getItemManageModule().getHaveItemParamInt(0x5BCF, 0);
            moduleAccesser->getWorkManageModule().setInt(swingFrames, 0x20000001);
            if (!moduleAccesser->getWorkManageModule().isFlag(0x1200000C)) {
                moduleAccesser->getItemManageModule().setHaveItemAction(1, 1.0f, 0);
            }
            if (moduleAccesser->getWorkManageModule().getInt(0x1000001D) == -1) {
                int handle = moduleAccesser->getSoundModule().playSENo3d((SndID)0x2072, false);
                moduleAccesser->getWorkManageModule().setInt(handle, 0x1000001D);
            }
        } else {
            moduleAccesser->getEffectModule().reqCommon(0x15, 0.0f);
            moduleAccesser->getItemManageModule().setHaveItemAction(1, 1.0f, 0);
            if (moduleAccesser->getWorkManageModule().getInt(0x1000001D) == -1) {
                int handle = moduleAccesser->getSoundModule().playSENo3d((SndID)0x2073, false);
                moduleAccesser->getWorkManageModule().setInt(handle, 0x1000001D);
            }
        }
        moduleAccesser->getControllerModule().setRumble(2, 0, false, 1);
    }
}

void ftStatusUniqProcessHammer::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0xE0) {
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        // BYTES: declaration order selects the native registers.
        soWorkManageModule* work;
        float target, acceleration, deceleration, speedLimit, speedScale, current;
        speedScale = ftUtil::getWalkSpeedMul(moduleAccesser);
        work = &moduleAccesser->getWorkManageModule();
        speedLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xC2E, 0);
        deceleration = soValueAccesser::getConstantFloat(moduleAccesser, 0xBBA, 0);
        acceleration = soValueAccesser::getConstantFloat(moduleAccesser, 0xBB9, 0);
        target = soValueAccesser::getConstantFloat(moduleAccesser, 0xBBB, 0);
        current = work->getFloat(0x21000004);
        fn_27_165458(moduleAccesser, 0x21000004, current, target, acceleration, deceleration, speedLimit, speedScale);
        Vec2f speed;
        speed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        speed.m_y = 0.0f;
        energy.m_speed = speed;
    }
    if (moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0x1B) {
        int holdFrames = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        if (!moduleAccesser->getWorkManageModule().isFlag(0x1200000D)) {
            soControllerModule* controller = &moduleAccesser->getControllerModule();
            u32 mask = soController::getButtonMask((soController::PadButton)0);
            int trigger = controller->getTrigger();
            if (trigger & mask) {
                soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
                int maxFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5A75, 0);
                holdFrames = maxFrames - work->getInt(0x20000004);
                Vec3f zero(0.0f, 0.0f, 0.0f);
                soEffectModule* effect = &moduleAccesser->getEffectModule();
                Vec3f pos = moduleAccesser->getPostureModule().getPos();
                effect->req((EfID)0xB, &pos, &zero, 1.0f, 0, -1);
            }
        }
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            ftHammerEnergyView* gravity = reinterpret_cast<ftHammerEnergyView*>(moduleAccesser->getKineticModule().getEnergy(1));
            if (holdFrames > 0) {
                holdFrames -= 1;
                if (holdFrames <= 0) {
                    moduleAccesser->getWorkManageModule().onFlag(0x1200000D);
                }
            }
            if (holdFrames > 0) {
                int counter = moduleAccesser->getWorkManageModule().getInt(0x20000003);
                int step = counter + 1;
                if (step > soValueAccesser::getConstantInt(moduleAccesser, 0x5A76, 0)) {
                    step = 0;
                    moduleAccesser->getWorkManageModule().incInt(0x20000004);
                }
                moduleAccesser->getWorkManageModule().setInt(step, 0x20000003);
                gravity->slot20();
                reinterpret_cast<u8*>(gravity)[6] = 1;
            } else {
                reinterpret_cast<u8*>(gravity)[6] = 0;
            }
        } else if (holdFrames > 0) {
            holdFrames -= 1;
        }
        moduleAccesser->getWorkManageModule().setInt(holdFrames, 0x20000002);
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            int state = moduleAccesser->getWorkManageModule().getInt(0x1000001B);
            if (state < soValueAccesser::getConstantInt(moduleAccesser, 0x5A74, 0)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000011);
                moduleAccesser->getEffectModule().reqCommon(0x16, 0.0f);
            }
        }
    }
}

void ftStatusUniqProcessHammer::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    int swingFrames = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    if (swingFrames > 0) {
        if (swingFrames - 1 <= 0) {
            moduleAccesser->getWorkManageModule().onFlag(0x1200000C);
            moduleAccesser->getItemManageModule().setHaveItemAction(0, 1.0f, 0);
        }
        moduleAccesser->getWorkManageModule().setInt(swingFrames - 1, 0x20000001);
    }
    if (moduleAccesser->getMotionModule().isLooped() == true) {
        ftLogTransactor::resetLogActionInfo(2, 0x68, 1, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
        moduleAccesser->getItemManageModule().updateHaveItemActionInfo(0);
    }
}

void ftStatusUniqProcessHammer::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessHammer::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (!ftUtil::isHammerStatus(moduleAccesser, nextStatusKind)) {
        if (moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0x1D) {
            moduleAccesser->getItemManageModule().setHaveItemAction(2, 1.0f, 0);
            moduleAccesser->getEffectModule().removeCommon(0x14);
        } else {
            moduleAccesser->getItemManageModule().setHaveItemAction(2, 1.0f, 0);
            moduleAccesser->getEffectModule().removeCommon(0x15);
        }
        moduleAccesser->getControllerModule().stopRumbleKind(2, 1);
    }
}

bool ftStatusUniqProcessHammer::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return false;
}
