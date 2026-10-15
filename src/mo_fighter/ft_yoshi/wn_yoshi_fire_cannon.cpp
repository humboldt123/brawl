#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/yoshi/wn_yoshi_fire_cannon.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

wnYoshiFireCannon::wnYoshiFireCannon(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiFireCannonData* data) :
    wnWeaponBuilder<wnYoshiFireCannonModuleAccesserBuildConfig>(
        "wnYoshiFireCannon", articleId, info,
        wnModuleAccesserBuildData(0x3F, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}


// The cannon ball is activated at the position with the given facing and flies along the angle (degrees) with the speed of the
// first parameter and no acceleration. HYPOTHESIS: meaning of the parameters.
void wnYoshiFireCannon::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float angle) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
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
    desc.life = 0;
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
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    kinetic.changeKinetic(2, m_moduleAccesser);
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    Vec2f direction(1.0f, 0.0f);
    direction.rot(&direction, 0.017453292f * angle);
    Vec2f speed = direction * m_param->unk0;
    speed.m_x *= lr;
    normal.m_speed = speed;
    normal.m_accel = Vec2f(0.0f, 0.0f);
    normal.m_brake = Vec2f(0.0f, 0.0f);
    normal.m_speedTarget = speed;
    normal.m_speedLimit = speed;
    normal.enable();
    updateNodeSRT();
}
