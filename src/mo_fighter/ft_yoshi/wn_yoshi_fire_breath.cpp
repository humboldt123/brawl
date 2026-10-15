#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/yoshi/wn_yoshi_fire_breath.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

namespace ftyoshi { template <class T> T ABS(T); }

wnYoshiFireBreath::wnYoshiFireBreath(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiFireBreathData* data) :
    wnWeaponBuilder<wnYoshiFireBreathModuleAccesserBuildConfig>(
        "wnYoshiFireBreath", articleId, info,
        wnModuleAccesserBuildData(0x3E, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// The breath is activated at the position with the given facing; its speed is the direction of the angle (degrees) scaled by
// the first parameter, its deceleration the same direction scaled by the second one. HYPOTHESIS: meaning of the parameters.
void wnYoshiFireBreath::activate(int founderTaskId, int team, const Vec3f* position, float lr, float angle) {
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
    Vec2f direction;
    direction.m_x = 1.0f;
    direction.m_y = 0.0f;
    direction.rot(&direction, 0.017453292f * angle);
    float speedScale = m_param->unk0;
    Vec2f speed = direction * speedScale;
    speed.m_x *= lr;
    float accelScale = m_param->unk4;
    Vec2f accel = direction * accelScale;
    accel.m_x *= -lr;
    accel.m_y *= -1.0f;
    normal.m_speed = speed;
    normal.m_accel = accel;
    Vec2f brake(ftyoshi::ABS(accel.m_x), ftyoshi::ABS(accel.m_y));
    normal.m_brake = brake;
    normal.m_speedTarget = speed;
    normal.m_speedLimit = speed;
    normal.enable();
    updateNodeSRT();
}
