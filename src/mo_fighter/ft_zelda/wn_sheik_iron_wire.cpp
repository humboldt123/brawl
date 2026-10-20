#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/sheik/wn_sheik_iron_wire.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessHad;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessShot;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessShotFall;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessFall;
extern soStatusUniqProcess g_wnSheikIronWireStatusUniqProcessTakeup;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnSheikIronWire::wnSheikIronWire(s32 articleId, const wnArticleBuildInfo& info, const wnSheikIronWireData* data) :
    wnWeaponBuilder<wnSheikIronWireModuleAccesserBuildConfig>(
        "SheikIronWire", articleId, info,
        wnModuleAccesserBuildData(0x46, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9];
    uniqProcs[0] = &g_wnSheikIronWireStatusUniqProcessHad;
    uniqProcs[1] = &g_wnSheikIronWireStatusUniqProcessShot;
    uniqProcs[2] = &g_wnSheikIronWireStatusUniqProcessShotFall;
    uniqProcs[3] = &g_wnSheikIronWireStatusUniqProcessFall;
    uniqProcs[4] = &g_wnSheikIronWireStatusUniqProcessTakeup;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}
