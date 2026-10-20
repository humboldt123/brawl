#define WN_STATUS_NO_TAIL // the null kinetic builder follows the status builder directly
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#pragma pool_data off
#include <wn/wn_module_accesser_builder.h>
#include <wn/dedede/wn_dedede_jethammer.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

wnDededeJethammer::wnDededeJethammer(s32 articleId, const wnArticleBuildInfo& info, const wnDededeJethammerData* data) :
    wnWeaponBuilder<wnDededeJethammerModuleAccesserBuildConfig>(
        "DededeJethammer", articleId, info,
        wnModuleAccesserBuildData(0x32, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    // Status 0 has no process; the other nine all use the hammer's process.
    soStatusUniqProcess* uniqProcs[10] = { 0 };
    uniqProcs[1] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[2] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[3] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[4] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[5] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[6] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[7] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[8] = &g_wnDededeJetHammerStatusUniqProcess;
    uniqProcs[9] = &g_wnDededeJetHammerStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 10);
    Weapon::deactivate(false);
}

// The hammer appears with the model resource of Dedede at the origin of his model; it starts in status 0.
// HYPOTHESIS: model constraint flag 8 is what makes the hammer follow the hand node.
void wnDededeJethammer::activate(int founderTaskId, int modelResourceId, float lr) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = modelResourceId;
    desc.unk8 = modelResourceId;
    desc.unkC = modelResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: activation copies position with integer moves.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = -1;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.flag4DBit7 = 1;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getLinkModule().setModelConstraintFlag(8);
}

// The fighter's animation commands move the hammer through its statuses by link events 0x83A-0x843 (status 0-9) and set its
// flags (0x22000000-0x22000002) with events 0x838, 0x839 and 0x844.
void wnDededeJethammer::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x838:
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000001);
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        break;
    case 0x839:
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        break;
    case 0x83A:
        m_moduleAccesser->getStatusModule().changeStatusRequest(0, m_moduleAccesser);
        break;
    case 0x83B:
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        break;
    case 0x83C:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    case 0x83D:
        m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
        break;
    case 0x83E:
        m_moduleAccesser->getStatusModule().changeStatusRequest(4, m_moduleAccesser);
        break;
    case 0x83F:
        m_moduleAccesser->getStatusModule().changeStatusRequest(5, m_moduleAccesser);
        break;
    case 0x840:
        m_moduleAccesser->getStatusModule().changeStatusRequest(6, m_moduleAccesser);
        break;
    case 0x841:
        m_moduleAccesser->getStatusModule().changeStatusRequest(7, m_moduleAccesser);
        break;
    case 0x842:
        m_moduleAccesser->getStatusModule().changeStatusRequest(8, m_moduleAccesser);
        break;
    case 0x843:
        m_moduleAccesser->getStatusModule().changeStatusRequest(9, m_moduleAccesser);
        break;
    case 0x844:
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
        break;
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}
