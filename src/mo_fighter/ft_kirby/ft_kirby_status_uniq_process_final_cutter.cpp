#include <ft/fighter.h>
#include <ft/kirby/ft_kirby_min.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the Final Cutter group of the extend parameters of Kirby: a pointer to floats at +8 (the first is the vertical
// speed scale of the rise, the second the multiplier of the horizontal speed limit).
struct ftKirbyFinalCutterExtend {
    u8 unk0[8];
    const float* unk8;
};

// HYPOTHESIS: two floats of the fighter attributes (0x74 limits the horizontal speed, 0x7c is the stable speed).
struct ftKirbyFinalCutterParam {
    u8 unk0[0x74];
    float unk74;
    u8 unk78[4];
    float unk7c;
};

// HYPOTHESIS: Kirby's Final Cutter status processes (the rise with the blade, the jump, the fall and the landing); status kinds
// 0x129-0x12c. The jump and the landing share the first steps with the start. Names are unknown.
class ftKirbyStatusUniqProcessFinalCutter : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutter() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftKirbyStatusUniqProcessFinalCutterJump : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutterJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftKirbyStatusUniqProcessFinalCutterFallLoop : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutterFallLoop() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftKirbyStatusUniqProcessFinalCutterLanding : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinalCutterLanding() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessFinalCutter::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    workManage.setInt(moduleAccesser->getSituationModule().getKind(), 0x20000002);
    kinetic.clearSpeedAll();
    kinetic.getEnergy(0)->disable();
    kinetic.getEnergy(1)->disable();
    kinetic.getEnergy(2)->disable();
    kinetic.getEnergy(3)->disable();
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    ftKirby& kirby = dynamic_cast<ftKirby&>(moduleAccesser->getStageObject());
    ftKirbyFinalCutterParam* param =
        reinterpret_cast<ftKirbyFinalCutterParam*>(g_ftCommonDataAccesser.getParam(kirby.getFtKind()));
    ftKirbyFinalCutterExtend* extend = reinterpret_cast<ftKirbyFinalCutterExtend*>(kirby.getExtendParam());
    controller.enable();
    Vec2f speed(0.0f, 0.0f);
    controller.resetEnergy(4, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    controller.mulXSpeedMax(param->unk74 * extend->unk8[1]);
    Vec2f stableSpeed(param->unk7c, 100.0f);
    controller.setStableSpeed(&stableSpeed);
}

void ftKirbyStatusUniqProcessFinalCutter::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0x129) {
        moduleAccesser->getEffectModule().killAll(2, true, true);
    }
}

void ftKirbyStatusUniqProcessFinalCutter::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessFinalCutterJump::initStatus(soModuleAccesser* moduleAccesser) {
    dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).getExtendParam();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    kinetic.changeKinetic(0x13, moduleAccesser);
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    ftKirby& kirby = dynamic_cast<ftKirby&>(moduleAccesser->getStageObject());
    ftKirbyFinalCutterParam* param =
        reinterpret_cast<ftKirbyFinalCutterParam*>(g_ftCommonDataAccesser.getParam(kirby.getFtKind()));
    ftKirbyFinalCutterExtend* extend = reinterpret_cast<ftKirbyFinalCutterExtend*>(kirby.getExtendParam());
    controller.enable();
    Vec2f speed(0.0f, 0.0f);
    controller.resetEnergy(4, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    controller.mulXSpeedMax(param->unk74 * extend->unk8[1]);
    Vec2f stableSpeed(param->unk7c, 100.0f);
    controller.setStableSpeed(&stableSpeed);
}

void ftKirbyStatusUniqProcessFinalCutterJump::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status < 0x12c && status >= 0x12a) {
        return;
    }
    moduleAccesser->getEffectModule().killAll(2, true, true);
}

void ftKirbyStatusUniqProcessFinalCutterJump::execStatus(soModuleAccesser* moduleAccesser) {
    ftKirbyFinalCutterExtend* extend =
        reinterpret_cast<ftKirbyFinalCutterExtend*>(dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).getExtendParam());
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
    if (workManage.getInt(0x20000002) == 2) {
        float speedY = motion.getSpeed().m_y;
        float scale = 1.0f;
        if (speedY > 0.0f) {
            scale = extend->unk8[0];
        }
        motion.unk40 = scale;
    }
}

void ftKirbyStatusUniqProcessFinalCutterFallLoop::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0x12b) {
        moduleAccesser->getEffectModule().killAll(2, true, true);
    }
}

void ftKirbyStatusUniqProcessFinalCutterLanding::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    kinetic.clearSpeedAll();
    kinetic.getEnergy(0)->disable();
    kinetic.getEnergy(1)->disable();
    kinetic.getEnergy(2)->disable();
    kinetic.getEnergy(3)->disable();
}

ftKirbyStatusUniqProcessFinalCutter g_ftKirbyStatusUniqProcessFinalCutter;
ftKirbyStatusUniqProcessFinalCutterJump g_ftKirbyStatusUniqProcessFinalCutterJump;
ftKirbyStatusUniqProcessFinalCutterFallLoop g_ftKirbyStatusUniqProcessFinalCutterFallLoop;
ftKirbyStatusUniqProcessFinalCutterLanding g_ftKirbyStatusUniqProcessFinalCutterLanding;
