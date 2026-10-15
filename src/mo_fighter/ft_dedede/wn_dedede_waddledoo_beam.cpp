#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/dedede/wn_dedede_waddledoo_beam.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

wnDededeWaddledooBeam::wnDededeWaddledooBeam(s32 articleId, const wnArticleBuildInfo& info, const wnDededeWaddledooBeamData* data) :
    wnWeaponBuilder<wnDededeWaddledooBeamModuleAccesserBuildConfig>(
        "wnDededeWaddledooBeam", articleId, info,
        wnModuleAccesserBuildData(0x34, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

// The beam is shot from the position along the angle (degrees) with the speed of the parameter block, mirrored by the facing.
void wnDededeWaddledooBeam::activate(int founderTaskId, int unused, int team, const Vec3f* position, float lr, float angle) {
    int life = m_param->unk4;
    wnActivateDesc desc;
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
    float speed = m_param->unk0;
    angle = 0.017453292f * angle;
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    sine = sin(angle);
    float cosine = cos(angle);
    Vec2f velocity(lr * (speed * cosine), speed * sine);
    normal.m_speed.m_x = velocity.m_x;
    normal.m_speed.m_y = velocity.m_y;
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}
