#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/ike/wn_ike_sword.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

// The status unique process of the sword (slot 3). HYPOTHESIS: unknown status.
extern soStatusUniqProcess g_wnIkeSwordStatusUniqProcessUnk3;

wnIkeSword::wnIkeSword(s32 articleId, const wnArticleBuildInfo& info, const wnIkeSwordData* data) :
    wnWeaponBuilder<wnIkeSwordModuleAccesserBuildConfig>(
        "wnIkeSword", articleId, info,
        wnModuleAccesserBuildData(0x3A, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[4];
    uniqProcs[0] = 0;
    uniqProcs[1] = 0;
    uniqProcs[2] = 0;
    uniqProcs[3] = &g_wnIkeSwordStatusUniqProcessUnk3;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 4);
    Weapon::deactivate(false);
}
