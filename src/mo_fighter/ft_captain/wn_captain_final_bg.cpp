#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/captain/wn_captain_final_bg.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

wnCaptainFinalBg::wnCaptainFinalBg(s32 articleId, const wnArticleBuildInfo& info, const wnCaptainFinalBgData* data) :
    wnWeaponBuilder<wnCaptainFinalBgModuleAccesserBuildConfig>(
        "wnCaptainFinalBg", articleId, info,
        wnModuleAccesserBuildData(0x8F, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// The background of the Final Smash is placed high above the stage (y = 3000) and turned
// by 90 degrees around the y axis.
void wnCaptainFinalBg::activate(int founderTaskId, int resourceId, int team) {
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
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unk38 = 1;
    Weapon::activate(&desc);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    posture.setPos(&Vec3f(0.0f, 3000.0f, 0.0f));
    posture.setRot(&Vec3f(0.0f, 90.0f, 0.0f), 0);
    updateNodeSRT();
}
