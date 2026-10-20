#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_rubber.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>

// The status unique process shared by the four active statuses of the rubber.
extern soStatusUniqProcess g_wnPopoRubberStatusUniqProcessCommon;

wnPopoRubber::wnPopoRubber(s32 articleId, const wnArticleBuildInfo& info, const wnPopoRubberData* data) :
    wnWeaponBuilder<wnPopoRubberModuleAccesserBuildConfig>(
        "wnPopoRubber", articleId, info,
        wnModuleAccesserBuildData(0x97, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[5] = { 0 };
    uniqProcs[1] = &g_wnPopoRubberStatusUniqProcessCommon;
    uniqProcs[2] = &g_wnPopoRubberStatusUniqProcessCommon;
    uniqProcs[3] = &g_wnPopoRubberStatusUniqProcessCommon;
    uniqProcs[4] = &g_wnPopoRubberStatusUniqProcessCommon;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 5);
    Weapon::deactivate(false);
}
