// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/masterhand/em_masterhand.h>
#include <so/so_value_accesser.h>

// Master Hand: an Enemy with six wnemSwHolder articles that use the shared activation of the Enemy unit. Its own code
// is the grab: the link events a caught fighter sends (em/masterhand/em_masterhand.h), the squeeze event its squeeze
// status sends to the fighter it holds, and the event its attacks send to its link parent (Crazy Hand, which takes
// the hit stop from it). Everything else in this unit is the module storage and the template instances of
// em/em_article_mediator.h.

// HYPOTHESIS: the statuses of Master Hand that the link events test, from what the handlers do in them: it catches in
// 0x2A, holds in 0x2B-0x2E (squeezing in 0x2C; a release lets go in 0x2D and 0x2E only) and lets go into 0x30.
enum {
    Masterhand_Status_Catch = 0x2a,
    Masterhand_Status_Hold1 = 0x2b,
    Masterhand_Status_HoldSqueeze = 0x2c,
    Masterhand_Status_Hold3 = 0x2d,
    Masterhand_Status_Hold4 = 0x2e,
    Masterhand_Status_LetGo = 0x30,
};

// HYPOTHESIS: the work variables the catch fills in: the caught fighter's task id and two counters cleared with it,
// and a flag that marks the catch.
static const u32 WorkInt_CaughtTaskId = 0x10000018;
static const u32 WorkInt_CatchCounter1 = 0x10000019;
static const u32 WorkInt_CatchCounter2 = 0x1000001a;
static const u32 WorkFlag_Caught = 0x1200000b;
// HYPOTHESIS: set while an attack of the hand is one the partner hand shares (its hit is reported to the link parent).
static const u32 WorkFlag_PartnerAttack = 0x12000005;

// ---- construction ---------------------------------------------------------------------------------------------------

emMasterhand::emMasterhand(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(36, 0),
    m_areaInstances(8, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emMasterhand::~emMasterhand() { }

// ---- events ---------------------------------------------------------------------------------------------------------

void emMasterhand::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    emHandLinkEventArgs* args = static_cast<emHandLinkEventArgs*>(eventInfo);
    switch (eventInfo->m_eventKind) {
    case Hand_LinkEvent_Catch:
        // A fighter touched the grab: hold it, unless the hand already holds something.
        if (moduleAccesser.getStatusModule().getStatusKind() == Masterhand_Status_Catch &&
            getHandCatchModule(moduleAccesser).isCatch() == false && sender->soGetKind() == StageObject_Fighter) {
            getHandCollisionCatchModule(moduleAccesser).unk18();
            args->_0x4 = true;
            args->unk1C = true;
            args->m_param = 5;
            getHandCatchModule(moduleAccesser).setCatch(-1);
            m_moduleAccesser->getWorkManageModule().setInt(sender->m_taskId, WorkInt_CaughtTaskId);
            m_moduleAccesser->getWorkManageModule().setInt(0, WorkInt_CatchCounter1);
            m_moduleAccesser->getWorkManageModule().setInt(0, WorkInt_CatchCounter2);
            m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_Caught);
            return;
        }
        break;
    case Hand_LinkEvent_Release:
        if (moduleAccesser.getStatusModule().getStatusKind() == Masterhand_Status_Hold3 ||
            moduleAccesser.getStatusModule().getStatusKind() == Masterhand_Status_Hold4) {
            bool isHeld = args->m_isHeld;
            getHandCatchModule(*m_moduleAccesser).unk40(0);
            getHandCatchModule(*m_moduleAccesser).catchCut(true, false);
            if (!isHeld) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(Masterhand_Status_LetGo, m_moduleAccesser);
            }
            return;
        }
        break;
    case Hand_LinkEvent_Escape:
        if (moduleAccesser.getStatusModule().getStatusKind() == Masterhand_Status_Hold1 ||
            moduleAccesser.getStatusModule().getStatusKind() == Masterhand_Status_HoldSqueeze ||
            moduleAccesser.getStatusModule().getStatusKind() == Masterhand_Status_Hold3 ||
            moduleAccesser.getStatusModule().getStatusKind() == Masterhand_Status_Hold4) {
            getHandCatchModule(*m_moduleAccesser).unk40(0);
            getHandCatchModule(*m_moduleAccesser).catchCut(true, false);
            m_moduleAccesser->getStatusModule().changeStatusRequest(Masterhand_Status_LetGo, m_moduleAccesser);
            return;
        }
        break;
    }
    Enemy::notifyEventLink(eventInfo, acc, sender, unk4);
}

void emMasterhand::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                           soModuleAccesser* acc) {
    // Entering the squeeze, tell the held fighter the squeeze parameters.
    if (statusKind == Masterhand_Status_HoldSqueeze && acc->getLinkModule().isLinked(0) == true) {
        emHandSqueezeEventArgs args;
        args.m_param.m_x = soValueAccesser::getValueFloat(acc, 0xfac, 0);
        args.m_param.m_y = 1.0f;
        args.m_param.m_z = soValueAccesser::getValueFloat(acc, 0xfad, 0);
        acc->getLinkModule().sendEventNodes(0, args, 0);
    }
    Enemy::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, acc);
}

bool emMasterhand::notifyEventCollisionAttackCheck(u32 flags) {
    bool result = Enemy::notifyEventCollisionAttackCheck(flags);
    if (moduleAccesser.getWorkManageModule().isFlag(WorkFlag_PartnerAttack)) {
        emHandEventArgs args(Hand_LinkEvent_PartnerAttackHit);
        m_moduleAccesser->getLinkModule().sendEventParents(3, args);
    }
    return result;
}
