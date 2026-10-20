#pragma once

// Donkey Kong's Barrel Conga article (article kind 0x71): the bongo barrel of the Konga Beat (final smash).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the article (1 float, id 4000). HYPOTHESIS: meaning of the parameter.
struct wnDonkeyBarrelCongaParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnDonkeyBarrelCongaData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnDonkeyBarrelCongaParam* m_param; // +0x34
};

class wnDonkeyBarrelCongaParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnDonkeyBarrelCongaParamAccesser(const wnDonkeyBarrelCongaParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x71) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnDonkeyBarrelCongaParamAccesser() { }
};

class wnDonkeyBarrelCongaModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnDonkeyBarrelConga : public wnWeaponBuilder<wnDonkeyBarrelCongaModuleAccesserBuildConfig> {
public:
    const wnDonkeyBarrelCongaParam* m_param;          // +0x2080
    wnDonkeyBarrelCongaParamAccesser m_paramAccesser; // +0x2084
    wnDonkeyBarrelConga(s32 articleId, const wnArticleBuildInfo& info, const wnDonkeyBarrelCongaData* data);
    virtual ~wnDonkeyBarrelConga() { }
};
static_assert(sizeof(wnDonkeyBarrelConga) == 0x2088, "Barrel Conga extent");
