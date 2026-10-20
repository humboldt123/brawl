#define WN_STATUS_NO_TAIL
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_hammer.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

wnKirbyHammer::wnKirbyHammer(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyHammerData* data) :
    wnWeaponBuilder<wnKirbyHammerModuleAccesserBuildConfig>(
        "KirbyHammer", articleId, info,
        wnModuleAccesserBuildData(0x6, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    Weapon::deactivate(false);
}
