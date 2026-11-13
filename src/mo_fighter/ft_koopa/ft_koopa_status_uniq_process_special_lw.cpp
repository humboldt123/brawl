#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_CTOR_NOINLINE // BYTES: every zero rotation passed to resetEnergy goes through the out-of-line constructor
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

#include <ft/koopa/ft_koopa_special_lw.h>

// HYPOTHESIS: the shared helpers of Bowser's down special (emitted at the start of this unit, called only by its status
// processes): the first clears the update flag (0x80) of the four kinetic energies, the second resets the work state,
// puts Bowser in the air and, unless he started in the air, marks the ground start (0x22000012) and stops the energies.
extern "C" void fn_102_D694(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(1)->disable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

extern "C" void fn_102_D75C(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int situation = moduleAccesser->getSituationModule().getKind();
    work.clearAll(2);
    work.offFlag(0x22000010);
    work.offFlag(0x22000011);
    moduleAccesser->getSituationModule().setKind(Situation_Air, false);
    moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    if (situation != 2) {
        work.onFlag(0x22000012);
        fn_102_D694(moduleAccesser);
    } else {
        work.offFlag(0x22000012);
    }
}

class ftKoopaStatusUniqProcessSpecialLwG : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessSpecialLwG() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftKoopaStatusUniqProcessSpecialLwA : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessSpecialLwA() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
};

void ftKoopaStatusUniqProcessSpecialLwG::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x1200003d)) {
        fn_102_D75C(moduleAccesser);
        work.offFlag(0x1200003d);
    }
}

// The motion energy follows the facing: when Bowser turns, the motion speed is mirrored.
void ftKoopaStatusUniqProcessSpecialLwG::execStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
    float lr = moduleAccesser->getPostureModule().getLr();
    if (lr != motion.m_facing) {
        motion.m_prevMotionSpeed.m_x = -motion.m_prevMotionSpeed.m_x;
        motion.m_facing = lr;
    }
}

void ftKoopaStatusUniqProcessSpecialLwA::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x1200003d)) {
        fn_102_D75C(moduleAccesser);
        work.offFlag(0x1200003d);
    }
    ftKoopaSpecialLwParam* param = getKoopaSpecialLwParam(Fighter_Koopa, 2);
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (!work.isFlag(0x22000012)) {
        speed.m_x *= param->m_airSpeedXMul;
        speed.m_y *= param->m_airSpeedYMul;
        fn_102_D694(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = param->m_gravity;
        gravity.m_fallSpeedMax = param->m_fallSpeedMax;
        gravity.unk1C = param->m_fallSpeedMax;
        gravity.enable();
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(9, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.m_brake = Vec2f(param->m_brakeX, 0.0f);
        stop.enable();
    } else {
        speed.m_y = param->m_dropSpeedY;
        fn_102_D694(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, param->m_dropSpeedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = 0.0f;
        gravity.enable();
        work.onFlag(0x22000011);
    }
}

void ftKoopaStatusUniqProcessSpecialLwA::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    ftKoopaSpecialLwParam* param = getKoopaSpecialLwParam(Fighter_Koopa, 2);
    if (!work.isFlag(0x22000011) && moduleAccesser->getMotionModule().isEnd()) {
        fn_102_D694(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, param->m_dropSpeedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = 0.0f;
        gravity.enable();
        work.onFlag(0x22000011);
    }
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (work.isFlag(0x22000011) || speed.m_y < param->m_dropSpeedY) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = param->m_dropSpeedY;
    }
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        if (work.isFlag(0x22000010)) {
            moduleAccesser->getGroundModule().setCliffCheck(soGroundShapeImpl::Cliff_Check_Always_Both_Sides, 0);
        } else {
            moduleAccesser->getGroundModule().setCliffCheck(soGroundShapeImpl::Cliff_Check_None, 0);
        }
    }
}

void ftKoopaStatusUniqProcessSpecialLwA::execFixPos(soModuleAccesser* moduleAccesser) {
    int situation = moduleAccesser->getSituationModule().getKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (situation != 2) {
        if (work.isFlag(0x22000010)) {
            fn_102_D694(moduleAccesser);
            moduleAccesser->getStatusModule().changeStatusRequest(0x11b, moduleAccesser);
        } else {
            moduleAccesser->getSituationModule().setKind(Situation_Air, false);
            moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
        }
    }
}

ftKoopaStatusUniqProcessSpecialLwA g_ftKoopaStatusUniqProcessSpecialLwA;
ftKoopaStatusUniqProcessSpecialLwG g_ftKoopaStatusUniqProcessSpecialLwG;
