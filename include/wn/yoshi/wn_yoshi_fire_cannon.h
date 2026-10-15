#pragma once

// Yoshi's fire cannon article (article kind 0x3F).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(3);

// The parameter block of the fire cannon (two floats, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnYoshiFireCannonParam {
    float unk0;
    float unk4;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnYoshiFireCannonData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnYoshiFireCannonParam* m_param; // +0x34
};

class wnYoshiFireCannonParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 0, 0, 0> {
public:
    wnYoshiFireCannonParamAccesser(const wnYoshiFireCannonParam* param) :
        wnExtendParamAccesser<3999, 2, 23999, 0, 0, 0>(0x3F) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
    }
    virtual ~wnYoshiFireCannonParamAccesser() { }
};

class wnYoshiFireCannonModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnYoshiFireCannon : public wnWeaponBuilder<wnYoshiFireCannonModuleAccesserBuildConfig> {
public:
    const wnYoshiFireCannonParam* m_param; // +0x1FA4
private:
    wnYoshiFireCannonParamAccesser m_paramAccesser; // +0x1FA8
public:
    wnYoshiFireCannon(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiFireCannonData* data);
    virtual ~wnYoshiFireCannon() { }
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float angle);
};
