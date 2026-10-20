#pragma once
// wnSnakeC4 article (article kind 0x18).
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(8);
// The parameter block of the article. HYPOTHESIS: layout of the untouched part.
struct wnSnakeC4Param {
    u8 unk0[4];
};
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnSnakeC4Data {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeC4Param* m_param; // +0x34
};
class wnSnakeC4ModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<7, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<8, soGeneralWorkBuildConfig<2, 2, 1>, 8, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 8, 0, 0, 1, 7, 0, 0, 1, 7, 0, 0, 1, 7, 0, 0> AnimCmdModuleSubBuildConfig;
};
class wnSnakeC4 : public wnWeaponBuilder<wnSnakeC4ModuleAccesserBuildConfig> {
public:
    const wnSnakeC4Param* m_param;
    wnSnakeC4(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4Data* data);
    virtual ~wnSnakeC4() { }
};
static_assert(sizeof(wnSnakeC4) == 0x2430, "wnSnakeC4 extent");
