#pragma once

// Meta Knight's Mantle article (article kind 0x6F).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the article (1 float, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnMetaknightMantleParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnMetaknightMantleData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnMetaknightMantleParam* m_param; // +0x34
};

class wnMetaknightMantleParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnMetaknightMantleParamAccesser(const wnMetaknightMantleParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x6F) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnMetaknightMantleParamAccesser() { }
};

class wnMetaknightMantleModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<17, 33, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<23, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfigNull,
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
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 23, 0, 0, 1, 23, 0, 0, 1, 23, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnMetaknightMantle : public wnWeaponBuilder<wnMetaknightMantleModuleAccesserBuildConfig> {
public:
    const wnMetaknightMantleParam* m_param; // +0x2074
private:
    wnMetaknightMantleParamAccesser m_paramAccesser; // +0x2078
public:
    wnMetaknightMantle(s32 articleId, const wnArticleBuildInfo& info, const wnMetaknightMantleData* data);
    virtual ~wnMetaknightMantle() { }
    // HYPOTHESIS: meaning of the third argument (stored in the descriptor next to the resource id).
    void activate(int founderTaskId, int resourceId, int unkC, int team);
};
