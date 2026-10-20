#define WN_STATUS_NO_TAIL
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_cook_hat.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

wnKirbyCookHat::wnKirbyCookHat(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyCookHatData* data) :
    wnWeaponBuilder<wnKirbyCookHatModuleAccesserBuildConfig>(
        "KirbyCookHat", articleId, info,
        wnModuleAccesserBuildData(0x92, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    // HYPOTHESIS: the four statuses of the hat share the default (none) uniq process.
    soStatusUniqProcess* processes[4] = {0};
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 4);
    Weapon::deactivate(false);
}
