#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/diddy/wn_diddy_explosion.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

wnDiddyExplosion::wnDiddyExplosion(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyExplosionData* data) :
    wnWeaponBuilder<wnDiddyExplosionModuleAccesserBuildConfig>(
        "DiddyExplosion", articleId, info,
        wnModuleAccesserBuildData(0x39, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1,
                                  &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    unk1F94(data->unk34) {
    soStatusUniqProcess* uniqProc = 0;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(&uniqProc, 1);
    Weapon::deactivate(false);
}

void wnDiddyExplosion::activate(int founderTaskId, int team, Vec3f* position, float lr) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, *position, lr, team, 0, 2, 0x80, 0, 0x35F,
                        0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(unk1F94[0], 0x10000005);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

void wnDiddyExplosion::onDeactivate() { }
