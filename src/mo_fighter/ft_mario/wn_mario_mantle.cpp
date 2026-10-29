#define SO_BUILD_DATA_SLOPE_LIMIT_EXTERNAL
// MATCH-ONLY: retain the native one-word null-input result.
#define IP_INPUT_BUTTON_RAW
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_ARTICLE_ANIMCMD_INT
#define WN_SHARED_ARRAYS_1
#define WN_TRANSITION_CTOR_INLINE
#define WN_WEAPON_NO_COMMON_ENERGIES
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/mario/wn_mario_mantle.h>
#include <wn/wn_activate_desc.h>
#include <so/motion/so_motion_change_param.h>

// The article inlines its work-storage setup; the fighter owns the weak copy.
inline soGeneralWorkSimple::soGeneralWorkSimple(s32* ints, u32 numInts, float* floats, u32 numFloats,
                                                u32* flags, u32 numFlags) :
    soGeneralWorkAbstract(false), m_intWorks(ints), m_intWorkSize(numInts), m_floatWorks(floats),
    m_floatWorkSize(numFloats), m_flagWorks(flags), m_flagWorkSize(numFlags) { }

// MATCH-ONLY: this constructor uses separate singleton addresses rather than a pooled BSS anchor.
#pragma push
#pragma pool_data off
wnMarioMantle::wnMarioMantle(s32 articleId, const wnArticleBuildInfo& info, const wnMarioMantleData* data) :
    wnWeaponBuilder<wnMarioMantleModuleAccesserBuildConfig>(
        "MarioMantle", articleId, info,
        wnModuleAccesserBuildData(1, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), 2, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    Weapon::deactivate(false);
}

#pragma pop

void wnMarioMantle::activate(int founderTaskId, int resourceId, int team, int situationKind) {
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
    // MATCH-ONLY: native activation copies the position as aggregate words.
    __memcpy(&desc.pos, &origin, sizeof(Vec3f));
#else
    desc.pos = origin;
#endif
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = true;
    desc.flagBit6 = true;
    desc.flagBit5 = false;
    desc.flagBit4 = false;
    desc.flagBit3 = true;
    desc.flagBit2 = false;
    desc.flagBit1 = false;
    desc.flagBit0 = false;
    desc.flag4DBit7 = false;
    desc.flag4DBit6 = false;
    Weapon::activate(&desc);
    soMotionChangeParam motion;
    motion.m_kind = situationKind != 0 ? 1 : 0;
    motion.m_frame = 0.0f;
    motion.m_rate = 1.0f;
    motion._12 = 0;
    motion._13 = 0;
    motion._14 = 0;
    motion._15 = 0;
    m_moduleAccesser->getMotionModule().changeMotion(&motion);
}
