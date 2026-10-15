#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/mario/wn_mario_huge_flame.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

// The status unique process of the fire (wn_mario_huge_flame_status_uniq_process_regular.cpp).
extern soStatusUniqProcess g_wnMarioHugeFlameStatusUniqProcessRegular;

wnMarioHugeFlame::wnMarioHugeFlame(s32 articleId, const wnArticleBuildInfo& info, const wnMarioHugeFlameData* data) :
    wnWeaponBuilder<wnMarioHugeFlameModuleAccesserBuildConfig>(
        "wnMarioHugeFlame", articleId, info,
        wnModuleAccesserBuildData(0x4C, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProc = &g_wnMarioHugeFlameStatusUniqProcessRegular;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(&uniqProc, 1);
    Weapon::deactivate(false);
}

void wnMarioHugeFlame::activate(int founderTaskId, int resourceId, int team, float lr) {
    Vec3f origin(0.0f, 0.0f, 0.0f);
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
    __memcpy(&desc.pos, &origin, sizeof(Vec3f));
#else
    desc.pos = origin;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(m_param->unk0, 0x10000005);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

// The flame leaves Mario's hand: it is placed at the position and no longer follows the hand node.
void wnMarioHugeFlame::toFly(Vec3f* position) {
    m_moduleAccesser->getPostureModule().initPos(position);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    updateNodeSRT();
}
