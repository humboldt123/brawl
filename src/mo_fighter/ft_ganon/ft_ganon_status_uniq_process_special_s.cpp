#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <ft/ft_kinetic_energy.h>
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: this translation unit holds Ganondorf's special-S (Flame Choke) and air special-S status processes. The
// work ids are unnamed where their meaning is unresolved; see the comments per use.

// HYPOTHESIS: link event the grab sends to its victim; the second payload word picks the victim's catched status.
struct ftGanonSpecialSLinkEvent : soLinkEventArgs {
    s32 m_payload;
    s32 m_unkC;
    s32 m_unk10;
    s32 m_unk14;
    float m_unk18;
    bool m_unk1C;
    ftGanonSpecialSLinkEvent(int kind, s32 payload, bool flag) : soLinkEventArgs(kind), m_payload(payload), m_unkC(-1), m_unk10(-1), m_unk14(-1), m_unk18(1.0f), m_unk1C(flag) { }
};

// The constructor-installed soCatchModuleImpl table places these methods after catchCut (see the Up-B Cling unit).
class ftGanonSpecialSCatchModuleView : public soCatchModule {
public:
    virtual void updatePosCling();
    virtual void updateLRCling();
    virtual void clingCut(bool cut);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
};

static inline ftGanonSpecialSCatchModuleView* getCatchModule(soModuleAccesser* moduleAccesser) {
    return static_cast<ftGanonSpecialSCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

// HYPOTHESIS: gravity and terminal speed of the energy that takes over once the move leaves its start.
static void setFallParams(soModuleAccesser* moduleAccesser, float gravityAccel, float fallSpeedMax);
static void updateAirCatchFall(soModuleAccesser* moduleAccesser);

class ftGanonStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftGanonStatusUniqProcessSpecialSCatch : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialSCatch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

class ftGanonStatusUniqProcessSpecialAirSCatch : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialAirSCatch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

class ftGanonStatusUniqProcessSpecialAirSFall : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialAirSFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftGanonStatusUniqProcessSpecialAirSEnd : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialAirSEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftGanonStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        // HYPOTHESIS: 0x22000011 is set by the script once the air start ends and gravity takes over.
        if (work.isFlag(0x22000011)) {
            float gravityAccel = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0);
            float fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7, 0);
            setFallParams(moduleAccesser, gravityAccel, fallSpeedMax);
        }
    }
}

void ftGanonStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x118: {
        // HYPOTHESIS: the ground catch scales the ground speed by the grab slide parameter.
        soKineticEnergyNormal& energy = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
        Vec2f speed = energy.getSpeed();
        float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0);
        energy.m_speed = speed * scale;
        break;
    }
    case 0x119:
        break;
    default:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            // Landing lag handed to the next status (same variables as the other fighters' special exits).
            float landingLag = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0);
            moduleAccesser->getWorkManageModule().setFloat(landingLag, 0x11000000);
            moduleAccesser->getWorkManageModule().onFlag(0x12000003);
        }
        break;
    }
}

void ftGanonStatusUniqProcessSpecialSCatch::initStatus(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    static_cast<ftGanonSpecialSCatchModuleView*>(modules->m_catchModule)->setCatch(-1);
    work.offFlag(0x22000015);
    work.offFlag(0x22000017);
    work.offFlag(0x22000018);
    work.offFlag(0x22000019);
    work.setInt(-1, 0x20000005);
}

void ftGanonStatusUniqProcessSpecialSCatch::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (!work.isFlag(0x22000015)) {
        // Tell the victim which catched status to enter, then remember who answered (same handshake as the Up-B throw).
        ftGanonSpecialSLinkEvent event(0x21, 0x1b1, true);
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
        moduleAccesser->getWorkManageModule().setInt(event.m_unkC, 0x20000003);
        moduleAccesser->getWorkManageModule().setInt(event.m_unk10, 0x20000004);
        work.onFlag(0x22000015);
    }
}

void ftGanonStatusUniqProcessSpecialSCatch::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    // HYPOTHESIS: variable float 0 is checked against 2.0; once above it, the linked fighter's task id is remembered.
    if (soValueAccesser::getVariableFloat(moduleAccesser, 0, 0) > 2.0f) {
        if (!work.isFlag(0x22000019)) {
            if (moduleAccesser->getLinkModule().isLinked(0)) {
                work.setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000005);
                work.onFlag(0x22000019);
            }
        }
    }
    // HYPOTHESIS: on the release flag the caught fighter is sent to status 0x49 (tumble).
    if (work.isFlag(0x22000017)) {
        if (!work.isFlag(0x22000018)) {
            gfTask* task = gfTask::getTask(work.getInt(0x20000005));
            if (task != NULL) {
                StageObject* stageObject = dynamic_cast<StageObject*>(task);
                if (stageObject != NULL) {
                    soModuleAccesser* victim = soExternalValueAccesser::getModuleAccesser(stageObject);
                    victim->getStatusModule().changeStatusRequest(0x49, victim);
                    work.onFlag(0x22000018);
                }
            }
        }
    }
}

void ftGanonStatusUniqProcessSpecialSCatch::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    getCatchModule(moduleAccesser)->catchCut(false, false);
}

bool ftGanonStatusUniqProcessSpecialSCatch::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    return getCatchModule(moduleAccesser)->checkDamage(moduleAccesser, damage);
}

void ftGanonStatusUniqProcessSpecialAirSCatch::initStatus(soModuleAccesser* moduleAccesser) {
    getCatchModule(moduleAccesser)->setCatch(-1);
    ftGanonSpecialSLinkEvent event(0x21, 0x1b2, true);
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getKineticModule().changeKinetic(0, moduleAccesser);
}

void ftGanonStatusUniqProcessSpecialAirSCatch::execStatus(soModuleAccesser* moduleAccesser) {
    updateAirCatchFall(moduleAccesser);
}

void ftGanonStatusUniqProcessSpecialAirSCatch::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus < 0x11c) {
        if (nextStatus >= 0x11a) {
            return;
        }
    }
    getCatchModule(moduleAccesser)->catchCut(false, false);
}

bool ftGanonStatusUniqProcessSpecialAirSCatch::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    return getCatchModule(moduleAccesser)->checkDamage(moduleAccesser, damage);
}

void ftGanonStatusUniqProcessSpecialAirSFall::initStatus(soModuleAccesser* moduleAccesser) {
    ftGanonSpecialSLinkEvent event(0x21, 0x1b3, true);
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
}

void ftGanonStatusUniqProcessSpecialAirSFall::execStatus(soModuleAccesser* moduleAccesser) {
    updateAirCatchFall(moduleAccesser);
}

void ftGanonStatusUniqProcessSpecialAirSFall::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x11b) {
        return;
    }
    getCatchModule(moduleAccesser)->catchCut(false, false);
}

void ftGanonStatusUniqProcessSpecialAirSEnd::initStatus(soModuleAccesser* moduleAccesser) {
    ftGanonSpecialSLinkEvent event(0x21, 0x1b4, true);
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(event.m_unkC, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(event.m_unk10, 0x20000004);
}

void ftGanonStatusUniqProcessSpecialAirSEnd::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    getCatchModule(moduleAccesser)->catchCut(false, false);
}

// HYPOTHESIS: work int 0x20000003 is the fall state the victim asked for and 0x20000004 the one already applied.
static void updateAirCatchFall(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int requested = work.getInt(0x20000003);
    int applied = work.getInt(0x20000004);
    if (requested != applied) {
        bool apply = true;
        float gravityAccel;
        float fallSpeedMax;
        switch (requested) {
        case 1:
            gravityAccel = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
            fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0);
            break;
        case 2:
            gravityAccel = soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0);
            fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0);
            break;
        default:
            apply = false;
            break;
        }
        if (apply) {
            setFallParams(moduleAccesser, gravityAccel, fallSpeedMax);
        }
        work.setInt(requested, 0x20000004);
    }
}

static void setFallParams(soModuleAccesser* moduleAccesser, float gravityAccel, float fallSpeedMax) {
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f speed;
    speed = gravity.getSpeed();
    Vec3f rotation;
    rotation = gravity.getRotation();
    gravity.resetEnergy(0, &speed, &rotation, moduleAccesser);
    gravity.m_gravity = -gravityAccel;
    gravity.unk1C = fallSpeedMax;
    gravity.m_fallSpeedMax = fallSpeedMax;
    gravity.enable();
}

ftGanonStatusUniqProcessSpecialS g_ftGanonStatusUniqProcessSpecialS;
ftGanonStatusUniqProcessSpecialSCatch g_ftGanonStatusUniqProcessSpecialSCatch;
ftGanonStatusUniqProcessSpecialAirSCatch g_ftGanonStatusUniqProcessSpecialAirSCatch;
ftGanonStatusUniqProcessSpecialAirSFall g_ftGanonStatusUniqProcessSpecialAirSFall;
ftGanonStatusUniqProcessSpecialAirSEnd g_ftGanonStatusUniqProcessSpecialAirSEnd;
