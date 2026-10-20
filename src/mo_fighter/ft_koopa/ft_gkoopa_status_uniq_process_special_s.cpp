#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_ASSIGN_NOINLINE // MATCH-ONLY: the original assigns the stop speeds through the out-of-line operator=
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_external_value_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <ft/fighter.h>
#include <gf/gf_task.h>
#include <so/catch/so_catch_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

#include <ft/koopa/ft_koopa_special_s.h>
// The first part of every exit: the victim (if still held) is let go and the throw flag set.
extern "C" void fn_102_17444(soModuleAccesser* moduleAccesser);

class ftGKoopaStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser*);
};

class ftGKoopaStatusUniqProcessSpecialSSquat : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSSquat() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftGKoopaStatusUniqProcessSpecialSJump : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSJump() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftGKoopaStatusUniqProcessSpecialSFall : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSFall() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

class ftGKoopaStatusUniqProcessSpecialSLanding : public soStatusUniqProcess {
public:
    virtual ~ftGKoopaStatusUniqProcessSpecialSLanding() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual bool checkDamage(soModuleAccesser*, void*);
};

void ftGKoopaStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x1200003d)) {
        work.clearAll(2);
        work.offFlag(0x22000011);
        work.offFlag(0x1200003d);
    }
}

void ftGKoopaStatusUniqProcessSpecialSSquat::initStatus(soModuleAccesser*) { }

void ftGKoopaStatusUniqProcessSpecialSSquat::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0x11f && moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        getCatchView(moduleAccesser).unk38(0);
        moduleAccesser->getWorkManageModule().onFlag(0x1200003e);
        ftKoopaLinkEventFlyingSlam event(0x26, -1, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    }
}

bool ftGKoopaStatusUniqProcessSpecialSSquat::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->getLinkModule().isLink(0) == true) {
        return getCatchView(moduleAccesser).unk3C();
    }
    return false;
}

void ftGKoopaStatusUniqProcessSpecialSJump::initStatus(soModuleAccesser* moduleAccesser) {
    ftKoopaSpecialSParam* param = getKoopaSpecialSParam(Fighter_GKoopa, 4);
    moduleAccesser->getSituationModule().setKind(Situation_Air, false);
    moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    fn_102_17444(moduleAccesser);
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
    motion.resetEnergy(6, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    motion.enable();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    float stick = moduleAccesser->getControllerModule().getStickX();
    float lr = moduleAccesser->getPostureModule().getLr();
    float lrSpeed = param->unk0 * lr;
    float stickSpeed = param->unk4 * stick;
    stop.resetEnergy(0x16, &Vec2f(stickSpeed + lrSpeed, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.m_speedTarget = Vec2f(param->unk24, 0.0f);
    stop.m_speedLimit = Vec2f(param->unk24, 0.0f);
    stop.enable();
    moduleAccesser->getWorkManageModule().onFlag(0x12000001);
}

void ftGKoopaStatusUniqProcessSpecialSJump::execStatus(soModuleAccesser* moduleAccesser) {
    ftKoopaSpecialSParam* param = getKoopaSpecialSParam(Fighter_GKoopa, 4);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f speed;
    Vec2f::copy(speed, stop.getSpeed());
    float add = 0.0f;
    if (moduleAccesser->getLinkModule().isLinked(0) == true) {
        gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
        gfTask* task = scheduler->getTask(moduleAccesser->getLinkModule().getNodeTaskId(0));
        if (StageObject* object = dynamic_cast<StageObject*>(task)) {
            float stick = soExternalValueAccesser::getModuleAccesser(object)->getControllerModule().getStickX();
            float damage = 0.0f;
            if (Fighter* fighter = dynamic_cast<Fighter*>(task)) {
                damage = ftExternalValueAccesser::getDamageModule(fighter)->getDamage(0);
            }
            float factor = param->unk14 - param->unk18 * damage;
            if (factor < param->unk1C) {
                factor = param->unk1C;
            }
            add += stick * factor;
        }
    }
    float stick = moduleAccesser->getControllerModule().getStickX();
    float factor = param->unk8 - param->unkC * moduleAccesser->getDamageModule().getDamage(0);
    if (factor < param->unk10) {
        factor = param->unk10;
    }
    add += stick * factor;
    if (add > param->unk20) {
        add = param->unk20;
    } else if (add < -param->unk20) {
        add = -param->unk20;
    }
    speed.m_x += add;
    if (speed.m_x > param->unk24) {
        speed.m_x = param->unk24;
    } else if (speed.m_x < -param->unk24) {
        speed.m_x = -param->unk24;
    }
    stop.m_speed = speed;
}

void ftGKoopaStatusUniqProcessSpecialSJump::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0x120 && moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        getCatchView(moduleAccesser).unk38(0);
        moduleAccesser->getWorkManageModule().onFlag(0x1200003e);
        ftKoopaLinkEventFlyingSlam event(0x26, -1, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        moduleAccesser->getWorkManageModule().offFlag(0x12000001);
    }
}

bool ftGKoopaStatusUniqProcessSpecialSJump::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->getLinkModule().isLink(0) == true) {
        return getCatchView(moduleAccesser).unk3C();
    }
    return false;
}

void ftGKoopaStatusUniqProcessSpecialSFall::initStatus(soModuleAccesser* moduleAccesser) {
    ftKoopaSpecialSParam* param = getKoopaSpecialSParam(Fighter_GKoopa, 4);
    fn_102_17444(moduleAccesser);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    gravity.resetEnergy(0, &Vec2f(0.0f, -param->unk28), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.m_gravity = 0.0f;
    gravity.enable();
    moduleAccesser->getWorkManageModule().onFlag(0x12000001);
}

void ftGKoopaStatusUniqProcessSpecialSFall::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status != 0x121 && moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        getCatchView(moduleAccesser).unk38(0);
        moduleAccesser->getWorkManageModule().onFlag(0x1200003e);
        ftKoopaLinkEventFlyingSlam event(0x26, -1, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000001);
}

bool ftGKoopaStatusUniqProcessSpecialSFall::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->getLinkModule().isLink(0) == true) {
        return getCatchView(moduleAccesser).unk3C();
    }
    return false;
}

void ftGKoopaStatusUniqProcessSpecialSLanding::initStatus(soModuleAccesser* moduleAccesser) {
    fn_102_17444(moduleAccesser);
    if (moduleAccesser->getLinkModule().isLinked(0) == true) {
        ftKoopaLinkEventFlyingSlam event(0x26, 0x1ba, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
        moduleAccesser->getWorkManageModule().setInt(event.unkC, 0x20000003);
        moduleAccesser->getWorkManageModule().setInt(event.unk10, 0x20000004);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
}

void ftGKoopaStatusUniqProcessSpecialSLanding::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    ftKoopaSpecialSParam* param = getKoopaSpecialSParam(Fighter_GKoopa, 4);
    if (work.isFlag(0x22000010)) {
        soLinkModule& link = moduleAccesser->getLinkModule();
        if (work.isFlag(0x22000011)) {
            ftKoopaLinkEventFlyingSlam event(0x0b, -1, true);
            link.sendEventNodes(0, event, 0);
            int taskId = link.getNodeTaskId(0);
            moduleAccesser->getCollisionAttackModule().hitAbsolute(0, 0x49, taskId, 0, 0);
        }
        fn_102_17444(moduleAccesser);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, param->unk30), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        float lr = moduleAccesser->getPostureModule().getLr();
        stop.resetEnergy(6, &Vec2f(param->unk2C * lr, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        work.offFlag(0x22000010);
    }
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = 0.0f;
    }
}

void ftGKoopaStatusUniqProcessSpecialSLanding::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        getCatchView(moduleAccesser).unk38(0);
        moduleAccesser->getWorkManageModule().onFlag(0x1200003e);
        ftKoopaLinkEventFlyingSlam event(0x26, -1, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    }
}

bool ftGKoopaStatusUniqProcessSpecialSLanding::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->getLinkModule().isLink(0) == true) {
        return getCatchView(moduleAccesser).unk3C();
    }
    return false;
}

ftGKoopaStatusUniqProcessSpecialS g_ftGKoopaStatusUniqProcessSpecialS;
ftGKoopaStatusUniqProcessSpecialSSquat g_ftGKoopaStatusUniqProcessSpecialSSquat;
ftGKoopaStatusUniqProcessSpecialSJump g_ftGKoopaStatusUniqProcessSpecialSJump;
ftGKoopaStatusUniqProcessSpecialSFall g_ftGKoopaStatusUniqProcessSpecialSFall;
ftGKoopaStatusUniqProcessSpecialSLanding g_ftGKoopaStatusUniqProcessSpecialSLanding;
