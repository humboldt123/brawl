#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/mario/wn_mario_pump_water.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
extern soStatusUniqProcess g_wnMarioPumpWaterStatusUniqProcessRegular;

wnMarioPumpWater::wnMarioPumpWater(s32 articleId, const wnArticleBuildInfo& info, const wnMarioPumpWaterData* data) :
    wnWeaponBuilder<wnMarioPumpWaterModuleAccesserBuildConfig>(
        "MarioPumpWater", articleId, info,
        wnModuleAccesserBuildData(0x48, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = &g_wnMarioPumpWaterStatusUniqProcessRegular;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    Weapon::deactivate(false);
}
