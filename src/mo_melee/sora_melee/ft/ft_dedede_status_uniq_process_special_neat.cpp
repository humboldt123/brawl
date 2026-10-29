#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/catch/so_catch_module.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

extern "C" {
// HYPOTHESIS: sora_melee walk helpers shared with the other walking statuses (see ft_kirby_status_uniq_process_special_n_eat.cpp);
// parameter names are guesses, the parameter order follows the argument registers of the target.
void fn_27_1652A8(soModuleAccesser* moduleAccesser, int motionKind, int speedWork, float parameter, float speedScale, int rateWork,
                  int flags);
void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, float speedScale, int rateWork);
void fn_27_165458(soModuleAccesser* moduleAccesser, float current, float target, float acceleration, float deceleration,
                  float speedLimit, int speedWork, float speedScale);
void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, float speedScale, int rateWork);
}

// HYPOTHESIS: the walk motions of the inhale (forward, backward, and the one the others blend from), for Dedede and for Kirby with the
// copied ability (sub kind 5).
static const int s_eatMotionKinds[3] = { 0x1e5, 0x1e4, 0x1e3 };
static const int s_eatMotionKindsKirby[3] = { 0x2d8, 0x2d7, 0x2d6 };

// HYPOTHESIS: a link event with a flag and a float, sent to the inhaled fighter (as in the Kirby version of this status).
struct ftDededeEatLinkEvent : soLinkEventArgs {
    float m_value;
    int unkC;
    bool m_flag;
    ftDededeEatLinkEvent(int kind) : soLinkEventArgs(kind), m_value(0.0f), m_flag(false) { }
};

class ftDededeStatusUniqProcessSpecialNEat : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialNEat() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    void setJump(soModuleAccesser* moduleAccesser);
    void execFixPosWait(soModuleAccesser* moduleAccesser);
};

ftDededeStatusUniqProcessSpecialNEat g_ftDededeStatusUniqProcessSpecialNEat;

// HYPOTHESIS: the original status kinds 0x11b-0x128 are the inhale (start, hold/walk, turn, swallow ...), as for Kirby.
void ftDededeStatusUniqProcessSpecialNEat::initStatus(soModuleAccesser* moduleAccesser) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(),
                                                                     moduleAccesser, true);
    moduleAccesser->getDamageModule().setReactionMul(moduleAccesser->getConstantFloatKirby(0xfa7));
    switch (status) {
    case 0x11b:
    case 0x121:
        moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
        break;
    case 0x11c: {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec2f speed(0.0f, 0.0f);
        speed.m_y = moduleAccesser->getConstantFloatKirby(0xfa3);
        gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        break;
    }
    case 0x125:
    case 0x126:
        moduleAccesser->getPostureModule().reverseLr();
        moduleAccesser->getKineticModule().clearSpeedAll();
        break;
    case 0x11e: {
        float speedScale = moduleAccesser->getPostureModule().getScale();
        speedScale *= moduleAccesser->getConstantFloatKirby(0xfa2);
        if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
            fn_27_1652A8(moduleAccesser, 0x2d8, 0x21000004, soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0), speedScale,
                         0x21000005, 0);
            fn_27_164BF0(moduleAccesser, s_eatMotionKindsKirby, 0x21000004, speedScale, 0x21000005);
        } else {
            fn_27_1652A8(moduleAccesser, 0x1e5, 0x21000004, soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0), speedScale,
                         0x21000005, 0);
            fn_27_164BF0(moduleAccesser, s_eatMotionKinds, 0x21000004, speedScale, 0x21000005);
        }
        break;
    }
    case 0x120:
        setJump(moduleAccesser);
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        break;
    case 0x122:
    case 0x123: {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec2f speed(0.0f, 0.0f);
        speed.m_y = -moduleAccesser->getConstantFloatKirby(0xfa3);
        gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        break;
    }
    }
}

void ftDededeStatusUniqProcessSpecialNEat::execStatus(soModuleAccesser* moduleAccesser) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser,
                                                                 true)) {
    case 0x11e: {
        float speedScale = moduleAccesser->getPostureModule().getScale();
        speedScale *= moduleAccesser->getConstantFloatKirby(0xfa2);
        soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
        u32 workBase = 0x21000000;
        fn_27_165458(moduleAccesser, workManage.getFloat(workBase + 4), soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xbb9, 0), soValueAccesser::getConstantFloat(moduleAccesser, 0xbba, 0),
                     soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0), workBase + 4, speedScale);
        if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
            fn_27_164E48(moduleAccesser, s_eatMotionKindsKirby, workBase + 4, speedScale, workBase + 5);
        } else {
            fn_27_164E48(moduleAccesser, s_eatMotionKinds, workBase + 4, speedScale, workBase + 5);
        }
        break;
    }
    }
}

void ftDededeStatusUniqProcessSpecialNEat::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNEat::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser,
                                                                 true)) {
    case 0x11b:
    case 0x11c:
        execFixPosWait(moduleAccesser);
        break;
    }
}

void ftDededeStatusUniqProcessSpecialNEat::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, status, moduleAccesser, true)) {
    case 0x11b:
    case 0x11c:
    case 0x11d:
    case 0x11e:
    case 0x11f:
    case 0x120:
    case 0x121:
    case 0x122:
    case 0x123:
    case 0x124:
    case 0x125:
    case 0x126:
        return;
    case 0x128: {
        ftDededeEatLinkEvent event(0x16);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        return;
    }
    }
    if (moduleAccesser->getLinkModule().isLinked(0) == true) {
        reinterpret_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, true);
    }
}

// HYPOTHESIS: the jump into the inhale: the horizontal speed follows the current speed and the stick, the vertical one the
// jump parameters.
void ftDededeStatusUniqProcessSpecialNEat::setJump(soModuleAccesser* moduleAccesser) {
    Vec2f speed;
    Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float scale = moduleAccesser->getConstantFloatKirby(0xfa4);
    float accel = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc4, 0);
    accel *= moduleAccesser->getControllerModule().getStickX();
    speed.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xbc6, 0);
    speed.m_x += accel;
    speed.m_x *= scale;
    float speedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc7, 0);
    if (__fabs(speed.m_x) > speedMax) {
        if (speed.m_x < 0.0f) {
            speedMax = -speedMax;
        }
        speed.m_x = speedMax;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200000a)) {
        float rise = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc5, 0) * soValueAccesser::getConstantFloat(moduleAccesser, 0xd7e, 0);
        speed.m_y *= soValueAccesser::getConstantFloat(moduleAccesser, 0xc46, 0);
        speed.m_y += rise;
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000006)) {
        float rise = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc8, 0);
        speed.m_y *= soValueAccesser::getConstantFloat(moduleAccesser, 0xc46, 0);
        speed.m_y += rise;
    } else {
        float rise = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc5, 0);
        speed.m_y *= soValueAccesser::getConstantFloat(moduleAccesser, 0xc46, 0);
        speed.m_y += rise;
    }
    speed.m_y *= scale;
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
    controller.resetEnergy(0, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    controller.m_accelMul = scale;
    controller.enable();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.enable();
}

// HYPOTHESIS: while the inhale is held the stick (work 0x21000006/7) moves Dedede up or down, or brakes him horizontally.
void ftDededeStatusUniqProcessSpecialNEat::execFixPosWait(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
        float stickY = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
        Vec2f stick(moduleAccesser->getWorkManageModule().getFloat(0x21000006), stickY);
        int jumpStatus;
        int fallStatus;
        if (moduleAccesser->getStageObject().soGetSubKind() == 5) {
            jumpStatus = 0x198;
            fallStatus = 0x19e;
        } else {
            jumpStatus = 0x11c;
            fallStatus = 0x122;
        }
        if (stick.m_y > 0.0f) {
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                moduleAccesser->getStatusModule().changeStatusRequest(jumpStatus, moduleAccesser);
            }
        } else if (stick.m_y < 0.0f) {
            if (moduleAccesser->getGroundModule().isPassableGround(0)) {
                moduleAccesser->getStatusModule().changeStatusRequest(fallStatus, moduleAccesser);
            }
        } else if (stick.m_x != 0.0f) {
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
                float stickX = stick.m_x;
                Vec2f brake(0.0f, 0.0f);
                float direction = (float)(stickX < 0.0f ? -1 : 1);
                float speed = moduleAccesser->getConstantFloatKirby(0xfa3);
                brake.m_x = speed * direction;
                stop.resetEnergy(0, &brake, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.enable();
            }
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000017);
    }
}
