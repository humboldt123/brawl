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
#include <wn/captain/wn_captain_blue_falcon.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/posture/so_posture_module_simple.h>

wnCaptainBlueFalcon::wnCaptainBlueFalcon(s32 articleId, const wnArticleBuildInfo& info, const wnCaptainBlueFalconData* data) :
    wnWeaponBuilder<wnCaptainBlueFalconModuleAccesserBuildConfig>(
        "wnCaptainBlueFalcon", articleId, info,
        wnModuleAccesserBuildData(0x8E, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnCaptainBlueFalcon::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float scale,
                                   bool attached) {
    float scaleCopy = scale;
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
    if (attached) {
        desc.unk38 = 5;
    }
    Weapon::activate(&desc);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    if (attached) {
        posture.setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
        updateNodeSRT();
        constraint(founderTaskId, 0, 0);
    } else {
        posture.setScale(scaleCopy);
    }
}
