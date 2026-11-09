#pragma once

// Zelda's Light Bow and Light Arrow, the Final Smash articles (article kinds 0x2b and 0x2c). The bow is only the pose
// holder of the move; the arrow flies with the common energies. Both classes are built in sora_melee, the fighter module
// only sees the opaque shells of ft/zelda/ft_zelda_article_pools.h.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/mario/wn_mario_fireball.h> // the null module builders of the sora_melee article unit mode
#endif

struct wnArticleBuildInfo;

// HYPOTHESIS: layout of the untouched part of the static article data (as the other articles' data blocks).
struct wnZeldaLightingBowData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const float* m_param; // +0x34, HYPOTHESIS: one float parameter
};

struct wnZeldaLightingBowArrowParam {
    s32 unk0;
    float unk4;
    float m_powerScale; // +8, read by the move status
};

struct wnZeldaLightingBowArrowData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnZeldaLightingBowArrowParam* m_param; // +0x34
};

class wnZeldaLightingBowParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnZeldaLightingBowParamAccesser(const float* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x2b) {
        m_floats[0] = param;
    }
    virtual ~wnZeldaLightingBowParamAccesser(); // BYTES: defined in the article unit, after the article constructor (emission order)
};
static_assert(sizeof(wnZeldaLightingBowParamAccesser) == 0x2C, "Light Bow parameter accesser extent");

class wnZeldaLightingBowArrowParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0> {
public:
    wnZeldaLightingBowArrowParamAccesser(const wnZeldaLightingBowArrowParam* param) :
        wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0>(0x2c) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->m_powerScale;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnZeldaLightingBowArrowParamAccesser(); // BYTES: defined in the article unit, after the article constructor (emission order)
};
static_assert(sizeof(wnZeldaLightingBowArrowParamAccesser) == 0x34, "Light Arrow parameter accesser extent");

class wnZeldaLightingBowModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
};

class wnZeldaLightingBowArrowModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
};


#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
// The sora_melee unit mode of the articles (the module builder constructor calls every getter out of line), as in
// wn_peach_kinopio.cpp / wn_mario_fireball.cpp: the member layout below is the one of the generic builder, spelled out so
// that the constructor can be defined by hand (wn_zelda_lighting_bow_builder_ctor.h).
// BYTES: the sora_melee article units construct the situation module builder with the module accesser first (the fighter
// RELs pass the unit id first, see ft/builder/ft_module_builders.h). Same type, so the symbols keep their names.
template <>
class soSituationModuleBuilder<soSituationModuleBuildConfig<soSituationModuleImpl> > {
    soSituationModuleImpl m_module;
public:
    soSituationModuleBuilder(soModuleAccesser* acc, s16 unitId, soEventObserverRegistrationDesc* regDesc) :
        m_module(unitId, acc, regDesc) { }
    soSituationModuleImpl* getModule() { return &m_module; }
};

#define WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS \
    soInsideEventManageModuleBuilder<BC::InsideEventManageModuleBuildConfig, BC::InsideEventManageModuleTypes> unk194; \
    soModuleAccesser m_moduleAccsr; \
    soHeapModuleBuilder<BC::HeapModuleBuildConfig> m_heapModuleBuilder; \
    soParamCustomizeModuleBuilder<BC::ParamCustomizeModuleBuildConfig> m_paramCustomizeModuleBuilder; \
    soResourceModuleBuilder<BC::ResourceModuleBuildConfig> m_resourceModuleBuilder; \
    soModelModuleBuilder<BC::ModelModuleBuildConfig> m_modelModuleBuilder; \
    soMotionModuleBuilder<BC::MotionModuleBuildConfig> m_motionBuilder; \
    soPostureModuleBuilder<BC::PostureModuleBuildConfig> m_postureModuleBuilder; \
    soGroundModuleBuilder<BC::GroundModuleBuildConfig> m_groundModuleBuilder; \
    soSituationModuleBuilder<BC::SituationModuleBuildConfig> m_situationModuleBuilder; \
    soTeamModuleBuilder<BC::TeamModuleBuildConfig> m_teamBuilder; \
    soCollisionAttackModuleBuilder<BC::CollisionAttackModuleBuildConfig> m_attackModuleBuilder; \
    soCollisionHitModuleBuilder<BC::CollisionHitModuleBuildConfig> m_hitModuleBuilder; \
    soCollisionShieldModuleBuilder<BC::CollisionShieldModuleBuildConfig> m_shieldModuleBuilder; \
    soCollisionReflectorModuleBuilder<BC::CollisionReflectorModuleBuildConfig> m_reflectorModuleBuilder; \
    soCollisionAbsorberModuleBuilder<soCollisionAbsorberModuleBuildConfigNull> m_absorberBuilder; \
    soCollisionCatchModuleBuilder<BC::CollisionCatchModuleBuildConfig> m_collisionCatchModuleBuilder; \
    soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigNull> m_searchBuilder; \
    soDamageModuleBuilder<BC::DamageModuleBuildConfig> m_damageModuleBuilder; \
    soCatchModuleBuilder<BC::CatchModuleBuildConfig> m_catchModuleBuilder; \
    soCaptureModuleBuilder<BC::CaptureModuleBuildConfig> m_captureModuleBuilder; \
    soStopModuleBuilder<BC::StopModuleBuildConfig> m_stopModuleBuilder; \
    soTurnModuleBuilder<BC::TurnModuleBuildConfig> m_turnModuleBuilder; \
    soShakeModuleBuilder<BC::ShakeModuleBuildConfig> m_shakeModuleBuilder; \
    soSoundModuleBuilder<BC::SoundModuleBuildConfig> m_soundModuleBuilder; \
    soLinkModuleBuilder<BC::LinkModuleBuildConfig> m_linkModuleBuilder; \
    soVisibilityModuleBuilder<BC::VisibilityModuleBuildConfig> m_visibilityModuleBuilder; \
    soControllerModuleBuilder<BC::ControllerModuleBuildConfig> m_controllerModuleBuilder; \
    soCameraModuleBuilder<BC::CameraModuleBuildConfig> m_cameraModuleBuilder; \
    soWorkManageModuleBuilder<BC::WorkManageModuleBuildConfig> m_workManageModuleBuilder; \
    soAnimCmdModuleBuilder<BC::AnimCmdModuleBuildConfig> m_animCmdBuilder; \
    soStatusModuleBuilder<BC::StatusModuleBuildConfig> m_statusBuilder; \
    soGeneralTermDecideModuleBuilder<soGeneralTermDecideModuleBuildConfig> m_generalTermDecideModuleBuilder; \
    soSwitchDecideModuleBuilder<soSwitchDecideModuleBuildConfig> m_switchDecideModuleBuilder; \
    soKineticModuleBuilder<BC::KineticModuleBuildConfig> m_kineticBuilder; \
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder; \
    BC::GenerateArticleManageModuleBuilder m_generateArticleBuilder; \
    soEffectModuleBuilder<BC::EffectModuleBuildConfig> m_effectModuleBuilder; \
    soComboModuleBuilder<BC::ComboModuleBuildConfig> m_comboModuleBuilder; \
    soAreaModuleBuilder<BC::AreaModuleBuildConfig> m_areaBuilder; \
    soTerritoryModuleBuilder<soTerritoryModuleBuildConfigNull> m_territoryModuleBuilder; \
    soTargetSearchModuleBuilder<soTargetSearchModuleBuildConfigNull> m_targetSearchModuleBuilder; \
    soPhysicsModuleBuilder<BC::PhysicsModuleBuildConfig> m_physicsModuleBuilder; \
    soSlopeModuleBuilder<BC::SlopeModuleBuildConfig> m_slopeModuleBuilder; \
    soShadowModuleBuilder<BC::ShadowModuleBuildConfig> m_shadowModuleBuilder; \
    soItemManageModuleBuilder<BC::ItemManageModuleBuildConfig> m_itemManageModuleBuilder; \
    soColorBlendModuleBuilder<BC::ColorBlendModuleBuildConfig> m_colorBlendModuleBuilder; \
    soJostleModuleBuilder<BC::JostleModuleBuildConfig> m_jostleModuleBuilder; \
    soAbnormalModuleBuilder<BC::AbnormalModuleBuildConfig> m_abnormalModuleBuilder; \
    soSlowModuleBuilder<BC::SlowModuleBuildConfig> m_slowModuleBuilder; \
    soReflectModuleBuilder<BC::ReflectModuleBuildConfig> m_reflectModuleBuilder; \
    soGlowModuleBuilder<BC::GlowModuleBuildConfig> m_glowModuleBuilder; \
    soDebugModuleBuilder<soDebugModuleBuildConfigNull> m_debugModuleBuilder;

template <>
class soModuleAccesserBuilder<wnZeldaLightingBowModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnZeldaLightingBowModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in wn_zelda_lighting_bow_builder_ctor.h (emission order)
};

template <>
class soModuleAccesserBuilder<wnZeldaLightingBowArrowModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnZeldaLightingBowArrowModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in wn_zelda_lighting_bow_builder_ctor.h (emission order)
};
#endif

class wnZeldaLightingBow : public wnWeaponBuilder<wnZeldaLightingBowModuleAccesserBuildConfig> {
    const float* m_param;                              // +0x1EF8
    wnZeldaLightingBowParamAccesser m_paramAccesser;   // +0x1EFC
public:
    wnZeldaLightingBow(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaLightingBowData* data);
    virtual ~wnZeldaLightingBow();
    // HYPOTHESIS: parameter names; the last one is passed on to Weapon::constraint (0x45 from the fighter).
    void activate(int founderTaskId, int resourceId, int team, int kind);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};


class wnZeldaLightingBowArrow : public wnWeaponBuilder<wnZeldaLightingBowArrowModuleAccesserBuildConfig> {
public:
    const wnZeldaLightingBowArrowParam* m_param;           // +0x1EF8
private:
    wnZeldaLightingBowArrowParamAccesser m_paramAccesser;  // +0x1EFC
public:
    wnZeldaLightingBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaLightingBowArrowData* data);
    virtual ~wnZeldaLightingBowArrow();
    // HYPOTHESIS: parameter names; the last one is passed on to Weapon::constraint (0x63 from the fighter).
    void activate(int founderTaskId, int resourceId, int team, int kind);
};

static_assert(sizeof(wnZeldaLightingBow) == 0x1F28, "Light Bow article extent");
static_assert(sizeof(wnZeldaLightingBowArrow) == 0x1F30, "Light Arrow article extent");
