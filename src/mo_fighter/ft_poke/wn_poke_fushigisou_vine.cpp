#define WN_WEAPON_BUILDER
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_STATUS_NO_TAIL
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/poke/wn_poke_fushigisou_vine.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>

// HYPOTHESIS: a data word of the REL (the node the vine model is attached with).
extern u32 lbl_116_data_C088;

// The status unique processes of the vine: all are the lasso ones of sora_melee (wn_lasso_status_uniq_process_*.cpp).
extern soStatusUniqProcess g_wnLassoStatusUniqProcessMotionShoot;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessPull;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnPokeFushigisouVine::wnPokeFushigisouVine(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouVineData* data) :
    wnWeaponBuilder<wnPokeFushigisouVineModuleAccesserBuildConfig>(
        "PokeFushigisouVine", articleId, info,
        wnModuleAccesserBuildData(75, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uniqProcs[1] = &g_wnLassoStatusUniqProcessMotionShoot;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[4] = &g_wnLassoStatusUniqProcessPull;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

void wnPokeFushigisouVine::activate(int founderTaskId, int resourceId, int unkC, int unk10, int unk14, int team, Vec3f* position,
                                    float lr, bool attach) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, position, sizeof(Vec3f));
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
    desc.unkC = unkC;
    desc.unk10 = unk10;
    desc.unk14 = unk14;
    desc.flag4DBit7 = 1;
    Weapon::activate(&desc);
    m_moduleAccesser->getKineticModule().changeKinetic(2, m_moduleAccesser);
    if (attach == true) {
        u32 nodeId = lbl_116_data_C088;
        wnLassoGetAltStatus(m_moduleAccesser)
            ->unk18Attach(m_moduleAccesser->getResourceModule().getResourceIdAccesser()->getBinResId(), 10, nodeId, nodeId, 11, 0,
                          0, -1);
        m_moduleAccesser->getWorkManageModule().setFlag(false, 0x22000000);
        m_moduleAccesser->getWorkManageModule().setInt(8, 0x10000005);
        wnLassoGetAltStatus(m_moduleAccesser)->unkDCFloat(m_moduleAccesser->getPostureModule().getScale());
        wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
        m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
    }
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}
