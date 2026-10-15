#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/lucas/wn_lucas_himohebi.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>

// HYPOTHESIS: a data word of the REL (the node or color the snake model is attached with).
extern u32 lbl_114_data_AED8;

// The status unique processes of the snake: the lasso ones live in sora_melee (wn_lasso_status_uniq_process_*.cpp), the exhaust
// one that extends the lasso one in this REL.
extern soStatusUniqProcess g_wnLassoStatusUniqProcessMotionShoot;
extern soStatusUniqProcess g_wnLucasHimohebiStatusUniqProcessExhaust;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessMotionPull;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnLucasHimohebi::wnLucasHimohebi(s32 articleId, const wnArticleBuildInfo& info, const wnLucasHimohebiData* data) :
    wnWeaponBuilder<wnLucasHimohebiModuleAccesserBuildConfig>(
        "LucasHimohebi", articleId, info,
        wnModuleAccesserBuildData(82, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uniqProcs[1] = &g_wnLassoStatusUniqProcessMotionShoot;
    uniqProcs[2] = &g_wnLucasHimohebiStatusUniqProcessExhaust;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[4] = &g_wnLassoStatusUniqProcessMotionPull;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

void wnLucasHimohebi::activate(int founderTaskId, int kind, int team, float lr, int attach, int plain) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = kind;
    desc.unk8 = kind;
    desc.unkC = kind;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, &zero, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    if (attach != 0) {
        soResourceModule& resource = m_moduleAccesser->getResourceModule();
        wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
        u32 nodeId = lbl_114_data_AED8;
        int noValue = 0;
        alt->unk18Attach(resource.getResourceIdAccesser()->getBinResId(), noValue, nodeId, nodeId, 1, 0, noValue, -1);
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        alt = wnLassoGetAltStatus(m_moduleAccesser);
        alt->unkDCFloat(posture.getScale());
        wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
        m_moduleAccesser->getWorkManageModule().setFlag(false, 0x22000000);
        m_moduleAccesser->getWorkManageModule().setInt(0xb, 0x10000005);
        m_moduleAccesser->getWorkManageModule().setInt(3, 0x10000006);
        m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    } else if (plain != 0) {
        // HYPOTHESIS: without an attached model the snake moves freely with these work values.
        m_moduleAccesser->getWorkManageModule().setInt(3, 0x20000000);
        m_moduleAccesser->getWorkManageModule().setFloat(19.63f, 0x21000000);
        m_moduleAccesser->getWorkManageModule().setFloat(100.0f, 0x21000001);
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk8, 0x21000002);
    }
}
