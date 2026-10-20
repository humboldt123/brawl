#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_dein.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

// The status unique processes of the article (wn_zelda_dein_status_uniq_process_*.cpp).
extern soStatusUniqProcess g_wnZeldaDeinStatusUniqProcessMove;
extern soStatusUniqProcess g_wnZeldaDeinStatusUniqProcessTame;

wnZeldaDein::wnZeldaDein(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaDeinData* data) :
    wnWeaponBuilder<wnZeldaDeinModuleAccesserBuildConfig>(
        "ZeldaDein", articleId, info,
        wnModuleAccesserBuildData(0x29, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = &g_wnZeldaDeinStatusUniqProcessMove;
    uniqProcs[1] = &g_wnZeldaDeinStatusUniqProcessTame;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}
