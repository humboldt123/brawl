#define WN_WEAPON_BUILDER
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/metaknight/wn_metaknight_mantle.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

wnMetaknightMantle::wnMetaknightMantle(s32 articleId, const wnArticleBuildInfo& info, const wnMetaknightMantleData* data) :
    wnWeaponBuilder<wnMetaknightMantleModuleAccesserBuildConfig>(
        "wnMetaknightMantle", articleId, info,
        wnModuleAccesserBuildData(0x6F, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnMetaknightMantle::activate(int founderTaskId, int resourceId, int unkC, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
#else
    desc.pos = position;
#endif
    desc.lr = 0.0f;
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
    desc.flag4DBit6 = 1;
    desc.unkC = unkC;
    desc.unk14 = 0;
    desc.unk10 = 1;
    Weapon::activate(&desc);
}
