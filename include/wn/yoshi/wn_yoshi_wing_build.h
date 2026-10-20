#pragma once
// wnYoshiWing article (article kind 0x3d).
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
// The parameter block of the article (1 float and 0 int parameters). HYPOTHESIS: meaning of the parameters.
struct wnYoshiWingParam {
    float unk0;
};
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnYoshiWingData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnYoshiWingParam* m_param; // +0x34
};
class wnYoshiWingParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnYoshiWingParamAccesser(const wnYoshiWingParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x3d) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnYoshiWingParamAccesser() { }
};
class wnYoshiWingModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0> AnimCmdModuleSubBuildConfig;
};
class wnYoshiWing : public wnWeaponBuilder<wnYoshiWingModuleAccesserBuildConfig> {
public:
    const wnYoshiWingParam* m_param;
    wnYoshiWingParamAccesser m_paramAccesser;
    wnYoshiWing(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiWingData* data);
    virtual ~wnYoshiWing() { }
};
