#pragma once

// Mario's Final Smash fire article (article kind 0x4C).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>


// The parameter block of the article (1 float and 1 int, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnMarioHugeFlameParam {
    s32 unk0;
    float unk4;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnMarioHugeFlameData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnMarioHugeFlameParam* m_param; // +0x34
};

class wnMarioHugeFlameParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 1, 0, 0> {
public:
    wnMarioHugeFlameParamAccesser(const wnMarioHugeFlameParam* param) : wnExtendParamAccesser<3999, 1, 23999, 1, 0, 0>(0x4C) {
        m_floats[0] = &param->unk4;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnMarioHugeFlameParamAccesser() { }
};

class wnMarioHugeFlameModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 2, 0, soCollisionAttackModuleImpl, 1, false, true>
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

class wnMarioHugeFlame : public wnWeaponBuilder<wnMarioHugeFlameModuleAccesserBuildConfig> {
public:
    const wnMarioHugeFlameParam* m_param; // +0x1F88
private:
    wnMarioHugeFlameParamAccesser m_paramAccesser; // +0x1F8C
public:
    wnMarioHugeFlame(s32 articleId, const wnArticleBuildInfo& info, const wnMarioHugeFlameData* data);
    virtual ~wnMarioHugeFlame() { }
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
    void activate(int founderTaskId, int resourceId, int team, float lr);
    void toFly(Vec3f* position);
};
