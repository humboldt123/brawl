#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_blizzard.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>
#include <mt/mt_prng.h>

wnPopoBlizzard::wnPopoBlizzard(s32 articleId, const wnArticleBuildInfo& info, const wnPopoBlizzardData* data) :
    wnWeaponBuilder<wnPopoBlizzardModuleAccesserBuildConfig>(
        "wnPopoBlizzard", articleId, info,
        wnModuleAccesserBuildData(0x65, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnPopoBlizzard::activate(int founderTaskId, int team, const Vec3f* position, float lr) {
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
    soMotionChangeParam motion;
    motion.m_kind = 0;
    motion.m_frame = 0.0f;
    motion.m_rate = 1.0f;
    motion._12 = 0;
    motion._13 = 0;
    motion._14 = 0;
    motion._15 = 0;
    m_moduleAccesser->getMotionModule().changeMotionRequest(&motion);
    wnKineticEnergyNormal* energy = dynamic_cast<wnKineticEnergyNormal*>(m_moduleAccesser->getKineticModule().getEnergy(0));
    if (energy != NULL) {
        float minAngle = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA2, 0);
        float maxAngle = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA3, 0);
        float range = maxAngle - minAngle;
        float angle = minAngle + randf() * range;
        float side = lr < 0.0f ? -1 : 1;
        angle = angle * side;
        angle = 0.017453292f * angle - 1.5707964f;
        while (angle < 0.0f) {
            angle += 6.2831855f;
        }
        while (angle > 6.2831855f) {
            angle -= 6.2831855f;
        }
        float speed = soValueAccesser::getConstantFloat(m_moduleAccesser, 4000, 0);
        soModuleAccesser* accesser = m_moduleAccesser;
        float sine = sin(angle);
        float cosine = cos(angle);
        energy->resetEnergy(0, &Vec2f(speed * cosine, speed * sine), &Vec3f(0.0f, 0.0f, 0.0f), accesser);
        float accel = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0);
        energy->m_accel = Vec2f(0.0f, accel);
        energy->enable();
    }
}
