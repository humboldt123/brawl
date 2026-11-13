#pragma once

#include <so/so_value_accesser.h>
#include <wn/wn_module_accesser_builder.h>

// HYPOTHESIS: tuning names, from how the article uses the block: activate flies it at `speed` along `angle` (radians) with
// the gravity and fall speed below, and a ground contact reflects the velocity scaled by `bounceScale`; below
// `minBounceSpeed` the fireball breaks up instead of bouncing.
struct wnMarioFireballParam {
    float gravity;         // 0x00: downward acceleration (negated into the energy's acceleration)
    float fallSpeedMax;    // 0x04: the energy's target (terminal) vertical speed
    float bounceScale;     // 0x08: velocity scale after a bounce
    float speed;           // 0x0C: launch speed
    float angle;           // 0x10: launch angle in radians
    int life;              // 0x14: lifetime in frames
    float minBounceSpeed;  // 0x18
};

// HYPOTHESIS: the Mario fireball article data (kind 0, shared with the Luigi fireball kind 149): the class
// info createArray reads the resource group number (+0x00, cast to u8 for the module build data) and the
// parameter pointer (+0x34, stored at article +0x1F40); the middle stays unreconstructed.
struct wnMarioFireballData {
    s32 m_resGroupNo;     // 0x00
    u8 unk4[0x30];        // 0x04
    const wnMarioFireballParam* m_param; // 0x34
};

#ifdef WN_MARIO_FIREBALL_UNIT
// HYPOTHESIS: source names of the null and decide module builders from the sora_melee symbol map.
struct soCollisionAbsorberModuleBuildConfigNull { };
template <class BC>
class soCollisionAbsorberModuleBuilder;
template <>
class soCollisionAbsorberModuleBuilder<soCollisionAbsorberModuleBuildConfigNull> {
public:
    soCollisionAbsorberModuleBuilder(soModuleAccesser*, int, u8) { }
    soCollisionShieldModule* getModule() { return (soCollisionShieldModule*)g_soCollisionAbsorberModuleNull; }
};

struct soCollisionSearchModuleBuildConfigNull { };
template <>
class soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigNull> {
public:
    soCollisionSearchModuleBuilder(soModuleAccesser*, int, u8) { }
    soCollisionSearchModule* getModule() { return (soCollisionSearchModule*)g_soCollisionSearchModuleNull; }
};

struct soDebugModuleBuildConfigNull { };
template <class BC>
class soDebugModuleBuilder {
public:
    soDebugModuleBuilder(soModuleAccesser*, soEventObserverRegistrationDesc*) { }
    void* getModule() { return g_soDebugModuleNull; }
};

struct soGeneralTermDecideModuleBuildConfig { };
template <class BC>
class soGeneralTermDecideModuleBuilder {
public:
    soGeneralTermDecideModuleBuilder() { }
    void* getModule() { return lbl_27_bss_170; }
};

struct soSwitchDecideModuleBuildConfig { };
template <class BC>
class soSwitchDecideModuleBuilder {
public:
    soSwitchDecideModuleBuilder() { }
    void* getModule() { return lbl_27_bss_188; }
};

struct soGenerateArticleManageModuleBuildConfigNull { };
template <class BC>
class soGenerateArticleManageModuleBuilder {
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser*) { }
    void* getModule() { return g_soGenerateArticleManageModuleNull; }
    soArticleMediator* getMediator() { return soSingletonHolder<soArticleMediatorNull>::getInstance(); }
};

struct soTerritoryModuleBuildConfigNull { };
template <class BC>
class soTerritoryModuleBuilder {
public:
    soTerritoryModuleBuilder(soModuleAccesser*, void*, s32*) { }
    void* getModule() { return g_soTerritoryModuleNull; }
};

struct soTargetSearchModuleBuildConfigNull { };
template <class BC>
class soTargetSearchModuleBuilder {
public:
    soTargetSearchModuleBuilder(soModuleAccesser*, void*) { }
    void* getModule() { return g_soTargetSearchModuleNull; }
};
#endif

// The module set the original unit instantiates (map 0x2da1f0-0x2dbba8): null situation, stop and
// visibility modules, a one-part fighter-category attack box, one motion slot, one status kind, link
// capacity 5 and four single-entry animation-command control units; the rest are the common defaults.
class wnMarioFireballModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    // The capacity-carrying config class: link capacity 5 (the one-argument config would fix it at 7
    // and change the extent).
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                     soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

#ifdef WN_MARIO_FIREBALL_UNIT
template <>
class soModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnMarioFireballModuleAccesserBuildConfig BC;
    soInsideEventManageModuleBuilder<BC::InsideEventManageModuleBuildConfig, BC::InsideEventManageModuleTypes> unk194; // +0x000
    soModuleAccesser m_moduleAccsr;                                                                                    // +0x7F8
    soHeapModuleBuilder<BC::HeapModuleBuildConfig> m_heapModuleBuilder;                                                // +0x8D8
    soParamCustomizeModuleBuilder<BC::ParamCustomizeModuleBuildConfig> m_paramCustomizeModuleBuilder;                  // +0x8F0
    soResourceModuleBuilder<BC::ResourceModuleBuildConfig> m_resourceModuleBuilder;                                    // +0x900
    soModelModuleBuilder<BC::ModelModuleBuildConfig> m_modelModuleBuilder;                                             // +0x930
    soMotionModuleBuilder<BC::MotionModuleBuildConfig> m_motionBuilder;                                                // +0xB10
    soPostureModuleBuilder<BC::PostureModuleBuildConfig> m_postureModuleBuilder;                                       // +0xD1C
    soGroundModuleBuilder<BC::GroundModuleBuildConfig> m_groundModuleBuilder;                                          // +0xDD0
    soSituationModuleBuilder<BC::SituationModuleBuildConfig> m_situationModuleBuilder;                                 // +0xE78
    soTeamModuleBuilder<BC::TeamModuleBuildConfig> m_teamBuilder;                                                      // +0xE7C
    soCollisionAttackModuleBuilder<BC::CollisionAttackModuleBuildConfig> m_attackModuleBuilder;                        // +0xEE0
    soCollisionHitModuleBuilder<BC::CollisionHitModuleBuildConfig> m_hitModuleBuilder;                                 // +0x10A4
    soCollisionShieldModuleBuilder<BC::CollisionShieldModuleBuildConfig> m_shieldModuleBuilder;                        // +0x10A5
    soCollisionReflectorModuleBuilder<BC::CollisionReflectorModuleBuildConfig> m_reflectorModuleBuilder;               // +0x10A6
    soCollisionAbsorberModuleBuilder<soCollisionAbsorberModuleBuildConfigNull> m_absorberBuilder;                      // +0x10A7
    soCollisionCatchModuleBuilder<BC::CollisionCatchModuleBuildConfig> m_collisionCatchModuleBuilder;                  // +0x10A8
    soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigNull> m_searchBuilder;                            // +0x10A9
    soDamageModuleBuilder<BC::DamageModuleBuildConfig> m_damageModuleBuilder;                                          // +0x10AA
    soCatchModuleBuilder<BC::CatchModuleBuildConfig> m_catchModuleBuilder;                                             // +0x10AB
    soCaptureModuleBuilder<BC::CaptureModuleBuildConfig> m_captureModuleBuilder;                                       // +0x10AC
    soStopModuleBuilder<BC::StopModuleBuildConfig> m_stopModuleBuilder;                                                // +0x10AD
    soTurnModuleBuilder<BC::TurnModuleBuildConfig> m_turnModuleBuilder;                                                // +0x10AE
    soShakeModuleBuilder<BC::ShakeModuleBuildConfig> m_shakeModuleBuilder;                                             // +0x10B0
    soSoundModuleBuilder<BC::SoundModuleBuildConfig> m_soundModuleBuilder;                                             // +0x10BC
    soLinkModuleBuilder<BC::LinkModuleBuildConfig> m_linkModuleBuilder;                                                // +0x1124
    soVisibilityModuleBuilder<BC::VisibilityModuleBuildConfig> m_visibilityModuleBuilder;                              // +0x1288
    soControllerModuleBuilder<BC::ControllerModuleBuildConfig> m_controllerModuleBuilder;                              // +0x1289
    soCameraModuleBuilder<BC::CameraModuleBuildConfig> m_cameraModuleBuilder;                                          // +0x128A
    soWorkManageModuleBuilder<BC::WorkManageModuleBuildConfig> m_workManageModuleBuilder;                              // +0x128C
    soAnimCmdModuleBuilder<BC::AnimCmdModuleBuildConfig> m_animCmdBuilder;                                             // +0x12C0
    soStatusModuleBuilder<BC::StatusModuleBuildConfig> m_statusBuilder;                                                // +0x1364
    soGeneralTermDecideModuleBuilder<soGeneralTermDecideModuleBuildConfig> m_generalTermDecideModuleBuilder;           // +0x1520
    soSwitchDecideModuleBuilder<soSwitchDecideModuleBuildConfig> m_switchDecideModuleBuilder;                          // +0x1521
    soKineticModuleBuilder<BC::KineticModuleBuildConfig> m_kineticBuilder;                                             // +0x1524
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder;                                             // +0x15D8
    soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> m_generateArticleBuilder;       // +0x1634
    soEffectModuleBuilder<BC::EffectModuleBuildConfig> m_effectModuleBuilder;                                          // +0x1638
    soComboModuleBuilder<BC::ComboModuleBuildConfig> m_comboModuleBuilder;                                             // +0x17C8
    soAreaModuleBuilder<BC::AreaModuleBuildConfig> m_areaBuilder;                                                      // +0x17C9
    soTerritoryModuleBuilder<soTerritoryModuleBuildConfigNull> m_territoryModuleBuilder;                               // +0x17CA
    soTargetSearchModuleBuilder<soTargetSearchModuleBuildConfigNull> m_targetSearchModuleBuilder;                      // +0x17CB
    soPhysicsModuleBuilder<BC::PhysicsModuleBuildConfig> m_physicsModuleBuilder;                                       // +0x17CC
    soSlopeModuleBuilder<BC::SlopeModuleBuildConfig> m_slopeModuleBuilder;                                             // +0x17CD
    soShadowModuleBuilder<BC::ShadowModuleBuildConfig> m_shadowModuleBuilder;                                           // +0x17CE
    soItemManageModuleBuilder<BC::ItemManageModuleBuildConfig> m_itemManageModuleBuilder;                              // +0x17CF
    soColorBlendModuleBuilder<BC::ColorBlendModuleBuildConfig> m_colorBlendModuleBuilder;                              // +0x17D0
    soJostleModuleBuilder<BC::JostleModuleBuildConfig> m_jostleModuleBuilder;                                          // +0x17D1
    soAbnormalModuleBuilder<BC::AbnormalModuleBuildConfig> m_abnormalModuleBuilder;                                    // +0x17D2
    soSlowModuleBuilder<BC::SlowModuleBuildConfig> m_slowModuleBuilder;                                                // +0x17D4
    soReflectModuleBuilder<BC::ReflectModuleBuildConfig> m_reflectModuleBuilder;                                       // +0x17EC
    soGlowModuleBuilder<BC::GlowModuleBuildConfig> m_glowModuleBuilder;                                                // +0x182C
    soDebugModuleBuilder<soDebugModuleBuildConfigNull> m_debugModuleBuilder;                                           // +0x1844

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser() { return &m_moduleAccsr; }
};
#endif

// A unit that builds its weapon builders without the two common energies (WN_WEAPON_NO_COMMON_ENERGIES, the Toad's
// spore) includes this header only for the sora_melee null builders above, so it does not see the article.
#ifndef WN_WEAPON_NO_COMMON_ENERGIES
// The bullet article. The shared class info placement-news it with the constructor below; defining it in
// the class body lets wnClassInfoImpl<0,...>::createArray inline it (the 0x3D8 createArray allocates the
// 0x1F44 extent and ends with the nine multiple-inheritance vptr writes).
class wnMarioFireball : public wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig> {
    const wnMarioFireballParam* m_param;  // 0x1F40
public:
#if defined(WN_MARIO_FIREBALL_CTOR_OUTLINE) || defined(WN_MARIO_FIREBALL_UNIT)
    // The Luigi fireball unit sees this constructor as a declaration only and calls the out-of-line
    // copy this unit emits (its createArray ends with the Luigi vptr writes after that call).
    wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data);
#else
    wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data) :
        wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig>(
            "MarioFireball", articleId, info,
            wnModuleAccesserBuildData(0, data, info.getHeapModule()->getInstanceHeap(),
                info.getHeapModule()->getNWModelInstanceHeap(),
                info.getHeapModule()->getNWMotionInstanceHeap(),
                static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
        m_param(data->m_param) {
        Weapon::deactivate(false);
    }
#endif
#if defined(WN_MARIO_FIREBALL_CTOR_OUTLINE) || defined(WN_MARIO_FIREBALL_UNIT)
    // This view borrows the shared Fireball implementation, including teardown.
    virtual ~wnMarioFireball();
#else
    virtual ~wnMarioFireball() { }
#endif
    // HYPOTHESIS: parameter meanings from the fighter transactor's call: the founder task, the effect resource, the team, the
    // start position, the first status kind and the facing.
    void activate(int founderTaskId, int resourceId, int team, Vec3f* __restrict pos, int statusKind, float lr);
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index);
};

#endif

static_assert(sizeof(soModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig>) == 0x1848,
              "Mario module builder extent");
static_assert(sizeof(wnModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig>) == 0x1DF8,
              "Mario article modules extent");
#ifndef WN_WEAPON_NO_COMMON_ENERGIES
static_assert(sizeof(wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig>) == 0x1F40,
              "Mario weapon builder extent");
static_assert(sizeof(wnMarioFireball) == 0x1F44, "Mario fireball article extent");
#endif
