#pragma once

// Yoshi's fire breath article (article kind 0x3E).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the fire breath (three floats, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnYoshiFireBreathParam {
    float unk0;
    float unk4;
    float unk8;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnYoshiFireBreathData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnYoshiFireBreathParam* m_param; // +0x34
};

class wnYoshiFireBreathParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0> {
public:
    wnYoshiFireBreathParamAccesser(const wnYoshiFireBreathParam* param) :
        wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>(0x3E) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
    }
    virtual ~wnYoshiFireBreathParamAccesser() { }
};

class wnYoshiFireBreathModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnYoshiFireBreath : public wnWeaponBuilder<wnYoshiFireBreathModuleAccesserBuildConfig> {
public:
    const wnYoshiFireBreathParam* m_param; // +0x1EF8
private:
    wnYoshiFireBreathParamAccesser m_paramAccesser; // +0x1EFC
public:
    wnYoshiFireBreath(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiFireBreathData* data);
    virtual ~wnYoshiFireBreath() { }
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
    void activate(int founderTaskId, int team, const Vec3f* position, float lr, float angle);
};
