#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/dedede/wn_dedede_star.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

wnDededeStar::wnDededeStar(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarData* data) :
    wnWeaponBuilder<wnDededeStarModuleAccesserBuildConfig>(
        "DededeStar", articleId, info,
        wnModuleAccesserBuildData(0x31, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

// The star is thrown from the position at the angle (degrees, mirrored when flipped) with the speed of 1.5 scaled by dir.
void wnDededeStar::activate(int founderTaskId, int unused, int team, const Vec3f* position, bool flip, float dir, float angle) {
    float lr;
    if (flip) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        angle = 180.0f - angle;
        lr = -1.0f;
    } else {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
        lr = 1.0f;
    }
    wnActivateDesc desc;
    s32 life = m_param->unk0;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = life;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    float sine;
    float speed = 1.5f;
    angle = 0.017453292f * angle;
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    sine = sin(angle);
    float cosine = cos(angle);
    Vec2f velocity(dir * (speed * cosine), speed * sine);
    normal.m_speed.m_x = velocity.m_x;
    normal.m_speed.m_y = velocity.m_y;
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}
