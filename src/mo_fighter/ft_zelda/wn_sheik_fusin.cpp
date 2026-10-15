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
#include <wn/zelda/wn_sheik_fusin.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/motion/so_motion_change_param.h>

wnSheikFusin::wnSheikFusin(s32 articleId, const wnArticleBuildInfo& info, const wnSheikFusinData* data) :
    wnWeaponBuilder<wnSheikFusinModuleAccesserBuildConfig>(
        "wnSheikFusin", articleId, info,
        wnModuleAccesserBuildData(0x47, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// Sheik's Final Smash article starts its first motion at its position.
void wnSheikFusin::activate(int founderTaskId, int team, const Vec3f* position, float lr) {
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
    desc.flag4DBit7 = 1;
    Weapon::activate(&desc);
    soMotionChangeParam changeParam;
    changeParam.m_kind = 0;
    changeParam.m_frame = 0.0f;
    changeParam.m_rate = 1.0f;
    changeParam._12 = 0;
    changeParam._13 = 0;
    changeParam._14 = 0;
    changeParam._15 = 0;
    m_moduleAccesser->getMotionModule().changeMotionRequest(&changeParam);
}
