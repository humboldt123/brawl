#pragma once

// Zelda's Din's Fire article that is thrown forward (article kind 0x2A).

#include <wn/wn_weapon_builder.h>

// The parameter block of the article: size and damage of the explosion grow over growFrames (read by the Bang status process).
// HYPOTHESIS: the names are descriptive.
struct wnZeldaDeinSParam {
    float growFrames;
    float initialSizeMul;
    float terminalSizeMul;
    float initialDamage;
    float damageGrowthRate;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnZeldaDeinSData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnZeldaDeinSParam* m_param; // +0x34
};

class wnZeldaDeinSModuleAccesserBuildConfig : public wnCommonBuildConfig {
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
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 3, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnZeldaDeinS : public wnWeaponBuilder<wnZeldaDeinSModuleAccesserBuildConfig> {
public:
    const wnZeldaDeinSParam* m_param; // +0x1F14
    wnZeldaDeinS(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaDeinSData* data);
    virtual ~wnZeldaDeinS() { }
    // HYPOTHESIS: the last parameter is the value of the work float 0x21000001.
    void activate(int founderTaskId, int team, const Vec3f* position, float lr, float unk);
};
