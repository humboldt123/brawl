#pragma once

// Popo's blizzard article (article kind 0x65): the ice crystal that the Blizzard (up special) throws out.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the article (4 floats, ids 4000-4003).
// HYPOTHESIS: meaning of the parameters.
struct wnPopoBlizzardParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPopoBlizzardData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPopoBlizzardParam* m_param; // +0x34
};

class wnPopoBlizzardParamAccesser : public wnExtendParamAccesser<3999, 4, 23999, 0, 0, 0> {
public:
    wnPopoBlizzardParamAccesser(const wnPopoBlizzardParam* param) : wnExtendParamAccesser<3999, 4, 23999, 0, 0, 0>(0x65) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
    }
    virtual ~wnPopoBlizzardParamAccesser() { }
};

class wnPopoBlizzardModuleAccesserBuildConfig : public wnCommonBuildConfig {
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
};

class wnPopoBlizzard : public wnWeaponBuilder<wnPopoBlizzardModuleAccesserBuildConfig> {
    const wnPopoBlizzardParam* m_param; // +0x1F10
    wnPopoBlizzardParamAccesser m_paramAccesser; // +0x1F14
public:
    wnPopoBlizzard(s32 articleId, const wnArticleBuildInfo& info, const wnPopoBlizzardData* data);
    virtual ~wnPopoBlizzard() { }
    // The crystal flies off at a random angle (within the constant range, mirrored by the facing) with the constant speed.
    void activate(int founderTaskId, int team, const Vec3f* position, float lr);
};
