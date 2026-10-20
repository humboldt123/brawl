#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

extern "C" {
// sora_melee helpers shared with the other walking statuses (see ft_donkey super lift walk); HYPOTHESIS: parameter names, the
// order of the parameters follows the argument registers of the target.
void fn_27_1652A8(soModuleAccesser* moduleAccesser, int motionKind, int speedWork, float parameter, float speedScale, int rateWork,
                  int flags);
void fn_27_164BF0(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, float speedScale, int rateWork);
void fn_27_165458(soModuleAccesser* moduleAccesser, float current, float target, float acceleration, float deceleration,
                  float speedLimit, int speedWork, float speedScale);
void fn_27_164E48(soModuleAccesser* moduleAccesser, const int* motionKinds, int speedWork, float speedScale, int rateWork);
}

// The walk motions of the inhale (forward, backward, and the one the others blend from).
static const int s_eatMotionKinds[3] = { 0x1e5, 0x1e4, 0x1e3 };

// HYPOTHESIS: a link event with a flag and a float, sent to the owner of the copied ability.
struct ftKirbyEatLinkEvent : soLinkEventArgs {
    float m_value;
    int unkC;
    bool m_flag;
    ftKirbyEatLinkEvent(int kind) : soLinkEventArgs(kind), m_value(0.0f), m_flag(false) { }
};

// HYPOTHESIS: the status kinds 0x11b-0x128 are the inhale (start, hold/walk, turn, swallow ...); names are unknown.
class ftKirbyStatusUniqProcessSpecialNEat : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNEat() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    void initInhaleJump(soModuleAccesser* moduleAccesser);
    void execInhaleStick(soModuleAccesser* moduleAccesser);
    void execInhaleWalk(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessSpecialNEat::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11b:
    case 0x121:
        moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
        break;
    case 0x11c: {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec2f speed(0.0f, 0.0f);
        speed.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
        gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        break;
    }
    case 0x124:
    case 0x125:
        moduleAccesser->getPostureModule().reverseLr();
        moduleAccesser->getKineticModule().clearSpeedAll();
        break;
    case 0x11e: {
        float speedScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0) * moduleAccesser->getPostureModule().getScale();
        fn_27_1652A8(moduleAccesser, 0x1e5, 0x21000004, soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0), speedScale,
                     0x21000005, 0);
        fn_27_164BF0(moduleAccesser, s_eatMotionKinds, 0x21000004, speedScale, 0x21000005);
        break;
    }
    case 0x120:
        initInhaleJump(moduleAccesser);
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        break;
    case 0x122: {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec2f speed(0.0f, 0.0f);
        speed.m_y = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
        gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        break;
    }
    }
}

void ftKirbyStatusUniqProcessSpecialNEat::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11e:
        execInhaleWalk(moduleAccesser);
        break;
    }
}

void ftKirbyStatusUniqProcessSpecialNEat::execStop(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialNEat::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11b:
    case 0x11c:
        execInhaleStick(moduleAccesser);
        break;
    }
}

void ftKirbyStatusUniqProcessSpecialNEat::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (status) {
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
        return;
    case 0x127:
    case 0x128: {
        ftKirbyEatLinkEvent event(0x16);
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
void ftKirbyStatusUniqProcessSpecialNEat::initInhaleJump(soModuleAccesser* moduleAccesser) {
    Vec2f speed;
    Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0);
    float accel = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc4, 0);
    accel *= moduleAccesser->getControllerModule().getStickX();
    speed.m_x = (speed.m_x * soValueAccesser::getConstantFloat(moduleAccesser, 0xbc6, 0) + accel) * scale;
    float speedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc7, 0);
    if (__fabs(speed.m_x) > speedMax) {
        if (speed.m_x < 0.0f) {
            speedMax = -speedMax;
        }
        speed.m_x = speedMax;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200000a)) {
        float rise = soValueAccesser::getConstantFloat(moduleAccesser, 0xd7e, 0);
        rise = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc5, 0) * rise;
        speed.m_y = speed.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xc46, 0) + rise;
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000006)) {
        float rise = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc8, 0);
        speed.m_y = speed.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xc46, 0) + rise;
    } else {
        float rise = soValueAccesser::getConstantFloat(moduleAccesser, 0xbc5, 0);
        speed.m_y = speed.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xc46, 0) + rise;
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

// HYPOTHESIS: while the inhale is held the stick (work 0x21000006/7) moves Kirby up or down, or brakes him horizontally.
void ftKirbyStatusUniqProcessSpecialNEat::execInhaleStick(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
        float stickY = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
        Vec2f stick(moduleAccesser->getWorkManageModule().getFloat(0x21000006), stickY);
        if (stickY > 0.0f) {
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x11c, moduleAccesser);
            }
        } else if (stickY < 0.0f) {
            if (moduleAccesser->getGroundModule().isPassableGround(0)) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x122, moduleAccesser);
            }
        } else if (stick.m_x != 0.0f) {
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
                Vec2f brake(0.0f, 0.0f);
                brake.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0) * (float)(stick.m_x < 0.0f ? -1 : 1);
                stop.resetEnergy(0, &brake, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.enable();
            }
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000017);
    }
}

// HYPOTHESIS: the walking part of the inhale (motion blending by the walk speed).
void ftKirbyStatusUniqProcessSpecialNEat::execInhaleWalk(soModuleAccesser* moduleAccesser) {
    float speedScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0) * moduleAccesser->getPostureModule().getScale();
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    u32 workBase = 0x21000000;
    fn_27_165458(moduleAccesser, workManage.getFloat(workBase + 4), soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xbb9, 0), soValueAccesser::getConstantFloat(moduleAccesser, 0xbba, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0), workBase + 4, speedScale);
    fn_27_164E48(moduleAccesser, s_eatMotionKinds, workBase + 4, speedScale, workBase + 5);
}

ftKirbyStatusUniqProcessSpecialNEat g_ftKirbyStatusUniqProcessSpecialNEat;
