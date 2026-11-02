#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/wario/wn_wario_garlic.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <types.h>

wnWarioGarlic::wnWarioGarlic(s32 articleId, const wnArticleBuildInfo& info, const wnWarioGarlicData* data) :
    wnWeaponBuilder<wnWarioGarlicModuleAccesserBuildConfig>(
        "WarioGarlic", articleId, info,
        wnModuleAccesserBuildData(0x61, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnWarioGarlic::activate(int founderTaskId, int resourceId, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, 0.0f, team, 0, 2,
                        0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}
