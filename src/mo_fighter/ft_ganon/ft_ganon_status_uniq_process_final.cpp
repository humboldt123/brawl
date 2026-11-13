#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <gf/gf_task.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <st/stage.h>
#include <wn/weapon.h>
#include <types.h>

// Ganondorf's Final Smash (Beast Ganon): the start, the charge and the return to his normal form.
// The beast is a weapon whose task id is kept in work int 0x20000000; it signals through its work flags 0x12000003 (the
// fighter goes airborne) and 0x12000004.
// HYPOTHESIS: the meaning of the work flags 0x22000010..0x22000015 (one-shot latches of this Final Smash).

class ftGanonStatusUniqProcessFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessFinalStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftGanonStatusUniqProcessFinalAttack : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessFinalAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    static void check_final_end(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender, int unk);
};

class ftGanonStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void check_weapon_signal(soModuleAccesser* moduleAccesser);
void to_air_rainbow_cruise(soModuleAccesser* moduleAccesser);

void ftGanonStatusUniqProcessFinalStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.onFlag(0x22000010);
    work.offFlag(0x22000011);
    work.setInt(-1, 0x20000000);
    work.offFlag(0x22000012);
    work.offFlag(0x22000013);
    work.offFlag(0x22000014);
    work.offFlag(0x22000015);
    work.onFlag(0x12000001);
    to_air_rainbow_cruise(moduleAccesser);
}

// Once the start is over, Ganondorf's own effects and model are hidden (the beast takes over).
void ftGanonStatusUniqProcessFinalStart::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (!work.isFlag(0x22000010) && !work.isFlag(0x22000011)) {
        moduleAccesser->getEffectModule().removeAll(8);
        moduleAccesser->getVisibilityModule().setWhole(0);
        work.onFlag(0x22000011);
    }
    check_weapon_signal(moduleAccesser);
}

void ftGanonStatusUniqProcessFinalStart::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftGanonStatusUniqProcessFinalStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) { }

void ftGanonStatusUniqProcessFinalAttack::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getVisibilityModule().setWhole(0);
    moduleAccesser->getWorkManageModule().onFlag(0x12000001);
    to_air_rainbow_cruise(moduleAccesser);
}

void ftGanonStatusUniqProcessFinalAttack::execStatus(soModuleAccesser* moduleAccesser) {
    check_weapon_signal(moduleAccesser);
}

// Link event from the beast (called by ftGanon::notifyEventLink): when the weapon that sent it is the one this Final Smash
// started, the attack status ends.
void ftGanonStatusUniqProcessFinalAttack::check_final_end(soLinkEventArgs* event, soModuleAccesser* moduleAccesser,
                                                          StageObject* sender, int unk) {
    soStatusModule& status = moduleAccesser->getStatusModule();
    if (status.getStatusKind() == 0x120 && sender->soGetKind() == 2) {
        Weapon& weapon = dynamic_cast<Weapon&>(*sender);
        u32 beastId = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        if (beastId == (u32)weapon.getTaskId()) {
            status.changeStatusRequest(0x121, moduleAccesser);
        }
    }
}

// Back to the normal form: visible again, and grounded when standing on something.
void ftGanonStatusUniqProcessFinalEnd::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getVisibilityModule().setWhole(1);
    moduleAccesser->getWorkManageModule().offFlag(0x12000001);
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (ground.isTouch(8, 0)) {
        moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        ground.setCorrect(soGroundShapeImpl::Correct_Ground, 0);
    }
}

// Reads the beast's work flags once each: 0x12000003 makes Ganondorf airborne, 0x12000004 is latched in 0x22000014.
void check_weapon_signal(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    gfTask* task = gfTask::getTask(work.getInt(0x20000000));
    if (task) {
        StageObject& beast = dynamic_cast<StageObject&>(*task);
        if (!work.isFlag(0x22000012) && !work.isFlag(0x22000013) &&
            soExternalValueAccesser::getWorkFlag(&beast, 0x12000003)) {
            moduleAccesser->getSituationModule().setKind(Situation_Air, false);
            work.onFlag(0x22000012);
        }
        if (!work.isFlag(0x22000014) && !work.isFlag(0x22000015) &&
            soExternalValueAccesser::getWorkFlag(&beast, 0x12000004)) {
            work.onFlag(0x22000014);
        }
    }
}

// On Rainbow Cruise the Final Smash runs in the air.
void to_air_rainbow_cruise(soModuleAccesser* moduleAccesser) {
    if (g_Stage && g_Stage->m_stageKind == 0x2f) {
        moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
}

ftGanonStatusUniqProcessFinalStart g_ftGanonStatusUniqProcessFinalStart;
ftGanonStatusUniqProcessFinalAttack g_ftGanonStatusUniqProcessFinalAttack;
ftGanonStatusUniqProcessFinalEnd g_ftGanonStatusUniqProcessFinalEnd;
