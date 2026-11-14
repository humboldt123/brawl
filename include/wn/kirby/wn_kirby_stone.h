#pragma once

// Kirby's stone article article (article kind 0x83).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

struct soVisibilityData;


// The parameter block of the article (1 float, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnKirbyStoneParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnKirbyStoneData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnKirbyStoneParam* m_param; // +0x34
    // HYPOTHESIS: per-form visibility, constraint nodes and activation data.
    struct {
        soVisibilityData* unk0;
        int unk4;
        int unk8;
        int unkC;
    }* unk38[5];
};

class wnKirbyStoneParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnKirbyStoneParamAccesser(const wnKirbyStoneParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x83) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnKirbyStoneParamAccesser() { }
};

class wnKirbyStoneModuleAccesserBuildConfig : public wnCommonBuildConfig {
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
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
};

class wnKirbyStone : public wnWeaponBuilder<wnKirbyStoneModuleAccesserBuildConfig> {
public:
    const wnKirbyStoneParam* m_param; // +0x20CC
private:
    wnKirbyStoneParamAccesser m_paramAccesser; // +0x20D0
public:
    wnKirbyStone(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyStoneData* data);
    void activate(int founderTaskId, int resourceId, int team, int kind);
#ifdef FT_KIRBY_ARTICLE_POOLS
    void activate(int founderTaskId, int resourceId, int team, int kind, float lr);
#endif
    virtual ~wnKirbyStone() { }
};
