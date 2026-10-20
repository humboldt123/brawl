#pragma once

// Peach's Kassar (Final Smash stage hazard) article (article kind 0x5C).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>


// The parameter block of the article (1 float, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnPeachKassarParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPeachKassarData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPeachKassarParam* m_param; // +0x34
};

class wnPeachKassarParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnPeachKassarParamAccesser(const wnPeachKassarParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x5C) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnPeachKassarParamAccesser() { }
};

class wnPeachKassarModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<9, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 2> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 9, 0, 0, 1, 9, 0, 0, 1, 9, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnPeachKassar : public wnWeaponBuilder<wnPeachKassarModuleAccesserBuildConfig> {
public:
    const wnPeachKassarParam* m_param; // +0x20CC
private:
    wnPeachKassarParamAccesser m_paramAccesser; // +0x20D0
public:
    wnPeachKassar(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKassarData* data);
    virtual ~wnPeachKassar() { }
    // HYPOTHESIS: argument meanings.
    void activate(int founderTaskId, int resourceId, int unkC, int team);
};
