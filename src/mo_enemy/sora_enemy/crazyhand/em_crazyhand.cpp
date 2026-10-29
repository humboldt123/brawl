// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/crazyhand/em_crazyhand.h>
#include <em/em_external_value_accesser.h>
#include <em/em_shot.h>
#include <so/so_value_accesser.h>

// Crazy Hand: Master Hand's partner, an Enemy with twenty wnemSwHolder articles and an activator of its own (the
// shared start vector). Its grab handles the same link events as Master Hand's (em/masterhand/em_masterhand.h), with
// its own statuses and without a status test on the catch, and it also answers three events of its partner: a status
// change, a work flag, and the hit stop of a shared attack. Everything else in this unit is the module storage and the
// template instances of em/em_article_mediator.h.

// HYPOTHESIS: the statuses of Crazy Hand that the link events test: it holds in 0x28-0x2A (squeezing in 0x28; a
// release lets go in 0x2A only) and lets go into 0x2B; 0x2F-0x31 are the attacks it shares with Master Hand.
enum {
    Crazyhand_Status_HoldSqueeze = 0x28,
    Crazyhand_Status_Hold2 = 0x29,
    Crazyhand_Status_Hold3 = 0x2a,
    Crazyhand_Status_LetGo = 0x2b,
    Crazyhand_Status_PartnerAttack1 = 0x2f,
    Crazyhand_Status_PartnerAttack2 = 0x30,
    Crazyhand_Status_PartnerAttack3 = 0x31,
};

// HYPOTHESIS: as in em_masterhand.cpp.
static const u32 WorkInt_CaughtTaskId = 0x10000018;
static const u32 WorkInt_CatchCounter1 = 0x10000019;
static const u32 WorkInt_CatchCounter2 = 0x1000001a;
static const u32 WorkFlag_Caught = 0x1200000b;
// HYPOTHESIS: the partner's next status (PartnerChangeStatus) and the flag it asks for (PartnerFlag).
static const u32 WorkInt_PartnerNextStatus = 0x10000006;
static const u32 WorkFlag_Partner = 0x12000006;

// ---- construction ---------------------------------------------------------------------------------------------------

emCrazyhand::emCrazyhand(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(38, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emCrazyhand::~emCrazyhand() { }

// ---- the activator --------------------------------------------------------------------------------------------------

// The shot starts along the shared start vector.
bool emCrazyhand::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// ---- events ---------------------------------------------------------------------------------------------------------

void emCrazyhand::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    emHandLinkEventArgs* args = static_cast<emHandLinkEventArgs*>(eventInfo);
    switch (eventInfo->m_eventKind) {
    case Hand_LinkEvent_Catch:
        // A fighter touched the grab: hold it, unless the hand already holds something.
        if (getHandCatchModule(moduleAccesser).isCatch() == false && sender->soGetKind() == StageObject_Fighter) {
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
        if (moduleAccesser.getStatusModule().getStatusKind() == Crazyhand_Status_Hold3) {
            bool isHeld = args->m_isHeld;
            getHandCatchModule(*m_moduleAccesser).unk40(0);
            getHandCatchModule(*m_moduleAccesser).catchCut(true, false);
            if (!isHeld) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(Crazyhand_Status_LetGo, m_moduleAccesser);
            }
            return;
        }
        break;
    case Hand_LinkEvent_Escape:
        if (moduleAccesser.getStatusModule().getStatusKind() == Crazyhand_Status_Hold2 ||
            moduleAccesser.getStatusModule().getStatusKind() == Crazyhand_Status_HoldSqueeze ||
            moduleAccesser.getStatusModule().getStatusKind() == Crazyhand_Status_Hold3) {
            getHandCatchModule(*m_moduleAccesser).unk40(0);
            getHandCatchModule(*m_moduleAccesser).catchCut(true, false);
            m_moduleAccesser->getStatusModule().changeStatusRequest(Crazyhand_Status_LetGo, m_moduleAccesser);
            return;
        }
        break;
    }
    // The events of the partner hand.
    switch (eventInfo->m_eventKind) {
    case Hand_LinkEvent_PartnerChangeStatus:
        m_moduleAccesser->getStatusModule().changeStatusRequest(acc->getWorkManageModule().getInt(WorkInt_PartnerNextStatus),
                                                                m_moduleAccesser);
        return;
    case Hand_LinkEvent_PartnerFlag:
        acc->getWorkManageModule().setFlag(true, WorkFlag_Partner);
        return;
    case Hand_LinkEvent_PartnerAttackHit:
        // The partner's attack connected: during a shared attack, stop as long as the partner does.
        if (m_moduleAccesser->getStatusModule().getStatusKind() == Crazyhand_Status_PartnerAttack1 ||
            m_moduleAccesser->getStatusModule().getStatusKind() == Crazyhand_Status_PartnerAttack2 ||
            m_moduleAccesser->getStatusModule().getStatusKind() == Crazyhand_Status_PartnerAttack3) {
            // The sender is always an enemy here (a reference cast).
            s32 hitStopFrame = emExternalValueAccesser::getHitStopFrame(&dynamic_cast<Enemy&>(*sender));
            acc->getStopModule().setHitStopFrame(hitStopFrame, true);
        }
        return;
    }
    Enemy::notifyEventLink(eventInfo, acc, sender, unk4);
}

void emCrazyhand::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                          soModuleAccesser* acc) {
    // Entering the squeeze, tell the held fighter the squeeze parameters.
    if (statusKind == Crazyhand_Status_HoldSqueeze && acc->getLinkModule().isLinked(0) == true) {
        emHandSqueezeEventArgs args;
        args.m_param.m_x = soValueAccesser::getValueFloat(acc, 0xfab, 0);
        args.m_param.m_y = 1.0f;
        args.m_param.m_z = soValueAccesser::getValueFloat(acc, 0xfac, 0);
        acc->getLinkModule().sendEventNodes(0, args, 0);
    }
    Enemy::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, acc);
}
